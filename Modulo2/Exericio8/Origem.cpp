/* M�dulo 2 - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 12/05/2023
 * Melissa Viana Kunst
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Prot�tipos das fun��es
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();
void resetAllRotate();
glm::mat4 rotateAll(glm::mat4 model);

// Dimens�es da janela
const GLuint WIDTH = 800, HEIGHT = 800;

// Par�metros de transforma��o
float scaleLevel = 0.5f;
GLfloat translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
bool rotateX = false, rotateY = false, rotateZ = false;
bool reverseRotateX = false, reverseRotateY = false, reverseRotateZ = false;

// C�digo fonte do Vertex Shader
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// C�digo fonte do Fragment Shader
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

// Fun��o MAIN
int main()
{
    // Inicializa��o da GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria��o da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modulo 2 - Melissa Kunst!", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros de fun��es da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Obtendo as informa��es de vers�o
    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version supported " << glGetString(GL_VERSION) << endl;

    // Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o programa de shader
    GLuint shaderID = setupShader();

    // Gerando um buffer simples, com a geometria de um tri�ngulo
    GLuint VAO = setupGeometry();

    glUseProgram(shaderID);

    glm::mat4 model = glm::mat4(1);
    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glEnable(GL_DEPTH_TEST);

    // Loop da aplica��o - "game loop"
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        model = glm::mat4(1);
        model = rotateAll(model);
        model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));
        model = glm::scale(model, glm::vec3(scaleLevel, scaleLevel, scaleLevel));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Desenhar o primeiro cubo
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glDrawArrays(GL_POINTS, 0, 72);
        glBindVertexArray(0);

        // Instanciar segundo cubo
        model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glDrawArrays(GL_POINTS, 0, 72);
        glBindVertexArray(0);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Limpeza de recursos
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

glm::mat4 rotateAll(glm::mat4 model) {
    float angle = (GLfloat)glfwGetTime();

    if (rotateX)
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    else if (reverseRotateX)
        model = glm::rotate(model, -angle, glm::vec3(1.0f, 0.0f, 0.0f));
    else if (rotateY)
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    else if (reverseRotateY)
        model = glm::rotate(model, -angle, glm::vec3(0.0f, 1.0f, 0.0f));
    else if (rotateZ)
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    else if (reverseRotateZ)
        model = glm::rotate(model, -angle, glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
        scaleLevel += 0.1f;
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
        scaleLevel -= 0.1f;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        resetAllRotate();
        rotateX = true;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        resetAllRotate();
        rotateY = true;
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        resetAllRotate();
        rotateZ = true;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_A: translateX -= 0.01f; break;
        case GLFW_KEY_D: translateX += 0.01f; break;
        case GLFW_KEY_W: translateY += 0.01f; break;
        case GLFW_KEY_S: translateY -= 0.01f; break;
        case GLFW_KEY_I: translateZ += 0.01f; break;
        case GLFW_KEY_J: translateZ -= 0.01f; break;
        default: break;
        }
    }
}

void resetAllRotate() {
    rotateX = rotateY = rotateZ = reverseRotateX = reverseRotateY = reverseRotateZ = false;
}

int setupShader() {
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int setupGeometry() {
    GLfloat vertices[] = {
        //lado verde 6 front face
         -0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //amarelo A
          0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //preto C
          0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //roxo E

         -0.5, -0.5, -0.5, 0.0, 1.0, 0.0, //amarelo A
          0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //roxo E
         -0.5,  0.5, -0.5, 0.0, 1.0, 0.0, //amarelo G

         //lado laranja 1 right face
         //x    y    z    r    g    b
          0.5, -0.5, -0.5, 1.0, 0.5, 0.25, // C
          0.5, -0.5,  0.5, 1.0, 0.5, 0.25, // D
          0.5,  0.5,  0.5, 1.0, 0.5, 0.25, // F

          0.5, -0.5, -0.5, 1.0, 0.5, 0.25, // C
          0.5,  0.5,  0.5, 1.0, 0.5, 0.25, // F
          0.5,  0.5, -0.5, 1.0, 0.5, 0.25, // E

          //lado preto e verde escuro 2 back face
           0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto D
          -0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto B
          -0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto H

           0.5, -0.5,  0.5, 0.0, 0.0, 0.0, //preto D
          -0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto H
           0.5,  0.5,  0.5, 0.0, 0.0, 0.0, //preto F

           //lado roxo 3 left face
          -0.5, -0.5,  0.5, 1.0, 0.0, 1.0, //roxo B
          -0.5, -0.5, -0.5, 1.0, 0.0, 1.0, //roxo a
          -0.5,  0.5, -0.5, 1.0, 0.0, 1.0, //roxo g

          -0.5, -0.5,  0.5, 1.0, 0.0, 1.0, //roxo B
          -0.5,  0.5, -0.5, 1.0, 0.0, 1.0, //roxo G
          -0.5,  0.5,  0.5, 1.0, 0.0, 1.0, //roxo H

          //lado amarelo 4 top face
         -0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo G
          0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo E
          0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo F

         -0.5,  0.5, -0.5, 1.0, 1.0, 0.0, //amarelo G
          0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo F
         -0.5,  0.5,  0.5, 1.0, 1.0, 0.0, //amarelo H

         //lado azul 5 bottom face
        -0.5, -0.5,  0.5, 0.0, 1.0, 1.0, //amarelo B
         0.5, -0.5,  0.5, 0.0, 1.0, 1.0, //roxo d
         0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //verde cinza C

         -0.5, -0.5, 0.5, 0.0, 1.0, 1.0, //amarelo B
          0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //roxo C
         -0.5, -0.5, -0.5, 0.0, 1.0, 1.0, //amarelo a
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return VAO;
}
