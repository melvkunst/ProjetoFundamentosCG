/*
*   Melissa Kunst
*   Computação Gráfica
*   Módulo 3
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"

using namespace std;

// Configurações da janela
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 700;

// Variáveis globais
vector<GLfloat> vertices, textures;
string mtlFilePath, textureFilePath;
bool rotateX = false, rotateY = false, rotateZ = false;
float scaleLevel = 0.5f;
GLfloat translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;

// Declarações de funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void setupWindow(GLFWwindow*& window);
void setupTransformacoes(glm::mat4& model);
void resetAllRotate();
void readFromMtl(const string& path);
void readFromObj(const string& path);
GLuint setupGeometry();
GLuint loadTexture(const string& path);

int main() {
    GLFWwindow* window;
    setupWindow(window);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    Shader shader("../shaders/sprite.vs", "../shaders/sprite.fs");

    readFromObj("../../3D_Models/Suzanne/bola.obj");
    readFromMtl("../../3D_Models/Suzanne/" + mtlFilePath);

    GLuint texID = loadTexture("../../3D_Models/Suzanne/" + textureFilePath);
    GLuint VAO = setupGeometry();

    glUseProgram(shader.ID);
    glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

    glm::mat4 projection = glm::rotate(glm::mat4(1), glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, false, glm::value_ptr(projection));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        glm::mat4 model = glm::mat4(1);
        setupTransformacoes(model);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, false, glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 3));
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

void setupWindow(GLFWwindow*& window) {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modulo 3 - Melissa Kunst", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version supported: " << glGetString(GL_VERSION) << endl;
}

void setupTransformacoes(glm::mat4& model) {
    float angle = static_cast<GLfloat>(glfwGetTime());
    model = glm::translate(glm::mat4(1), glm::vec3(translateX, translateY, translateZ));

    if (rotateX)
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    if (rotateY)
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    if (rotateZ)
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(scaleLevel));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    const float scaleStep = 0.1f;
    const float translateStep = 0.01f;

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        scaleLevel += scaleStep;
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        scaleLevel -= scaleStep;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        resetAllRotate();
        rotateX = true;
    }
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        resetAllRotate();
        rotateY = true;
    }
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        resetAllRotate();
        rotateZ = true;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_D:
            translateX -= translateStep;
            break;
        case GLFW_KEY_A:
            translateX += translateStep;
            break;
        case GLFW_KEY_W:
            translateY += translateStep;
            break;
        case GLFW_KEY_S:
            translateY -= translateStep;
            break;
        case GLFW_KEY_I:
            translateZ += translateStep;
            break;
        case GLFW_KEY_J:
            translateZ -= translateStep;
            break;
        }
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        resetAllRotate();
        translateX = translateY = translateZ = 0.0f;
        scaleLevel = 0.5f;
    }
}

void resetAllRotate() {
    rotateX = rotateY = rotateZ = false;
}

void readFromMtl(const string& path) {
    ifstream mtlFile(path);
    if (!mtlFile.is_open()) {
        cerr << "Failed to open the MTL file." << endl;
        return;
    }

    string line;
    while (getline(mtlFile, line)) {
        if (line.find("map_Kd") == 0) {
            istringstream iss(line);
            string ignore;
            iss >> ignore >> textureFilePath;
        }
    }
}

void readFromObj(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open the OBJ file." << endl;
        return;
    }

    vector<glm::vec3> temp_vertices, temp_normals;
    vector<glm::vec2> temp_textures;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_textures.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            unsigned int vertexIndex, uvIndex, normalIndex;
            char slash;
            for (int i = 0; i < 3; ++i) {
                iss >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
                glm::vec3 vertex = temp_vertices[vertexIndex - 1];
                glm::vec2 uv = temp_textures[uvIndex - 1];

                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.z);
                textures.push_back(uv.x);
                textures.push_back(uv.y);
            }
        }
        else if (prefix == "mtllib") {
            iss >> mtlFilePath;
        }
    }
}

GLuint setupGeometry() {
    GLuint VAO, VBO[2];

    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    return VAO;
}

GLuint loadTexture(const string& path) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cerr << "Failed to load texture" << endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}
