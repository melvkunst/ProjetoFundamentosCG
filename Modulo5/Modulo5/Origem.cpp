/*
*   Melissa Kunst
*   Computação Gráfica
*   Módulo 5
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

using namespace std;

// Configuração da janela
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void setupWindow(GLFWwindow*& window);
void resetAllRotate();
void setupTransformations(glm::mat4& model);
void setupShader(Shader shader);

// Configuração da geometria
void readFromObj(string path);
void readFromMtl(string path);
int setupGeometry();
int loadTexture(string path);

// Valores do OBJ
vector<GLfloat> totalVertices;
vector<GLfloat> vertexPositions;
vector<GLfloat> textureCoords;
vector<GLfloat> normals;

// Valores dos arquivos
string mtlFilePath = "";
string textureFilePath = "";
string basePath = "../../3D_Models/Suzanne/";
string objFileName = "CuboTextured.obj";

// Valores de iluminação
vector<GLfloat> ka;
vector<GLfloat> ks;
float ns;

bool firstMouse = true;
float lastX, lastY;
float sensitivity = 0.05f;
float pitch = 0.0, yaw = -90.0;

// Tamanho da janela
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Parâmetros de transformação
bool rotateX = false;
bool rotateY = false;
bool rotateZ = false;
float scale = 0.5f;


GLfloat translateX = 0.0f;
GLfloat translateY = 0.0f;
GLfloat translateZ = 0.0f;

Camera camera;

int main()
{
    GLFWwindow* window;

    // Configuração da janela
    setupWindow(window);

    // Obter o tamanho do framebuffer
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Carregar shaders
    Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

    // Ler arquivos OBJ e MTL
    readFromObj(basePath + objFileName);
    readFromMtl(basePath + mtlFilePath);

    // Carregar textura
    GLuint textureID = loadTexture(basePath + textureFilePath);

    // Configurar geometria
    GLuint VAO = setupGeometry();

    // Usar o programa de shader
    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    // Inicializar o objeto
    Mesh object;
    object.initialize(VAO, (totalVertices.size() / 8), &shader);

    // Configurar shaders
    setupShader(shader);

    // Habilitar teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Inicializar câmera
    camera.initialize(&shader, width, height);

    // Loop de renderização
    while (!glfwWindowShouldClose(window))
    {
        // Verificar eventos
        glfwPollEvents();

        // Limpar buffers de cor e profundidade
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Configurar matriz de modelo
        glm::mat4 model = glm::mat4(1.0f);
        setupTransformations(model);
        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Atualizar câmera
        camera.update();

        // Ativar textura e desenhar o objeto
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        object.draw(textureID);

        glBindTexture(GL_TEXTURE_2D, 0);

        // Trocar buffers
        glfwSwapBuffers(window);
    }

    // Limpar recursos
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

void setupShader(Shader shader) {
    shader.setVec3("ka", ka[0], ka[1], ka[2]);
    shader.setFloat("kd", 0.7f);
    shader.setVec3("ks", ks[0], ks[1], ks[2]);
    shader.setFloat("q", ns);

    shader.setVec3("lightPos", -2.0f, 100.0f, 2.0f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
}

void readFromMtl(string path)
{
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return;
    }

    std::string line;

    // Ler cada linha do arquivo .mtl
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            // Verificar prefixos e carregar dados apropriados
            if (prefix == "map_Kd") {
                iss >> textureFilePath;
            }
            else if (prefix == "Ka") {
                glm::vec3 tempKa;
                iss >> tempKa.x >> tempKa.y >> tempKa.z;
                ka = { tempKa.x, tempKa.y, tempKa.z };
            }
            else if (prefix == "Ks") {
                glm::vec3 tempKs;
                iss >> tempKs.x >> tempKs.y >> tempKs.z;
                ks = { tempKs.x, tempKs.y, tempKs.z };
            }
            else if (prefix == "Ns") {
                iss >> ns;
            }
        }
    }

    file.close();
}

int setupGeometry()
{
    GLuint VBO, VAO;

    // Gerar buffer de vértices e array de vértices
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    // Bind do VBO e VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, totalVertices.size() * sizeof(GLfloat), totalVertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);

    // Configurar atributos de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Configurar atributos de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Configurar atributos de normais
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // Desvincular VBO e VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void readFromObj(string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return;
    }

    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempTextures;
    std::vector<glm::vec3> tempNormals;

    std::string line;

    // Ler cada linha do arquivo .obj
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            // Verificar prefixos e carregar dados apropriados
            if (prefix == "v") {
                glm::vec3 values;
                iss >> values.x >> values.y >> values.z;
                tempVertices.push_back(values);
            }
            else if (prefix == "vt") {
                glm::vec2 values;
                iss >> values.x >> values.y;
                tempTextures.push_back(values);
            }
            else if (prefix == "vn") {
                glm::vec3 values;
                iss >> values.x >> values.y >> values.z;
                tempNormals.push_back(values);
            }
            else if (prefix == "f") {
                unsigned int vertexIndex[3], textIndex[3], normalIndex[3];
                char slash;

                // Ler índices de vértices, texturas e normais
                for (int i = 0; i < 3; ++i) {
                    iss >> vertexIndex[i] >> slash >> textIndex[i] >> slash >> normalIndex[i];

                    // Verificar se os índices estão dentro dos limites
                    if (vertexIndex[i] > tempVertices.size() || textIndex[i] > tempTextures.size() || normalIndex[i] > tempNormals.size()) {
                        std::cerr << "Index out of bounds in OBJ file at line: " << line << std::endl;
                        return;
                    }

                    // Carregar vértices, texturas e normais nos vetores correspondentes
                    glm::vec3 vertex = tempVertices[vertexIndex[i] - 1];
                    glm::vec3 normal = tempNormals[normalIndex[i] - 1];
                    glm::vec2 texture = tempTextures[textIndex[i] - 1];

                    totalVertices.insert(totalVertices.end(), { vertex.x, vertex.y, vertex.z });
                    totalVertices.insert(totalVertices.end(), { texture.x, texture.y });
                    totalVertices.insert(totalVertices.end(), { normal.x, normal.y, normal.z });
                }
            }
            else if (prefix == "mtllib") {
                iss >> mtlFilePath;
            }
        }
    }

    file.close();
}

int loadTexture(string path)
{
    std::cout << "Texture path: " << path << std::endl;

    GLuint textureID;

    // Gerar e bind de textura
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configurar parâmetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carregar imagem da textura
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void setupWindow(GLFWwindow*& window) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Criar janela GLFW
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 4 - Melissa Kunst", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // Configurar callback de teclado
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Desabilita o desenho do cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Obter informações de versão do OpenGL
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;
}

void setupTransformations(glm::mat4& model) {
    // Calcular ângulo de rotação baseado no tempo
    float angle = static_cast<GLfloat>(glfwGetTime());

    // Aplicar rotações
    if (rotateX) {
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    else if (rotateY) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (rotateZ) {
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // Translação
    model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));

    // Escala
    model = glm::scale(model, glm::vec3(scale, scale, scale));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        rotateX = true;
        rotateY = false;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = true;
        rotateZ = false;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        rotateX = false;
        rotateY = false;
        rotateZ = true;

    }
    camera.setCameraPos(key);
}

void resetAllRotate() {
    // Resetar todas as rotações
    rotateX = false;
    rotateY = false;
    rotateZ = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(window, xpos, ypos);
}
