#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

void frameBufferResize(GLFWwindow *window, int width, int height);

struct ShaderCode{
    std::string vertex;
    std::string fragment;
};

static ShaderCode parseShaderCode(const std::string &filePath);

static std::string readFile(const std::string &filePath);

static unsigned int createProgram(const std::string &vertex, const std::string &fragment);

static unsigned int compileShader(unsigned int type, const std::string &code);

static unsigned int linkShader(const std::string &vertexCode, const std::string &fragmentCode);

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow *window{glfwCreateWindow(600, 400, "GlExample", nullptr, nullptr)};

    if (!window) {
        std::cout << "Failed to init glfw window." << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferResize);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to init glad." << std::endl;
        return -1;
    }

//    float vertices[] {
//            .5f, .0f, .0f,
//            .0f, .5f, .0f,
//            .0f, -.5f, .0f,
//            -.5f, .0f, .0f
//    };
    float vertices [] {
        .3f, .7f,
        -.3f, .7f,
        .0f, .2f,
        -.2f, 0,
        .2f, 0,
    };

    unsigned int indices[] {
        0,1, 2,
        2, 3, 4
    };

    float vertices2 [] {
        -.5f, -.7f,
        .5f, -.7f,
        .0f, -.2f
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    unsigned int VAO2, VBO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    auto shaderCode{parseShaderCode("../resources/shaders/basic.shader")};
    auto program{linkShader(shaderCode.vertex, shaderCode.fragment)};
    auto program2{createProgram(readFile("../resources/shaders/simple.vert"), readFile("../resources/shaders/simple.frag"))};

    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(.5f, .6f, .7f, .8f);
        glClear(GL_COLOR_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glUseProgram(program);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(VAO2);
        glUseProgram(program2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void frameBufferResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static ShaderCode parseShaderCode(const std::string &filePath) {
    std::ifstream file{filePath};

    if (!file.is_open()) {
        std::cout << "Failed to open fila: " << filePath << std::endl;
    }

    enum class ShaderType {
        None = -1,
        Vertex = 0,
        Fragment = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type{ShaderType::None};

    while (getline(file, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::Vertex;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::Fragment;
            }
        } else {
            if (type != ShaderType::None) {
                ss[int(type)] << line << std::endl;
            }
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static std::string readFile(const std::string &filePath) {
    std::ifstream file{filePath};
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filePath;
    }

    std::string line;
    std::stringstream result;

    while (getline(file, line)) {
        result << line << std::endl;
    }

    return result.str();
}

static unsigned int createProgram(const std::string &vertex, const std::string &fragment) {
    auto vs{compileShader(GL_VERTEX_SHADER, vertex)};
    auto fs{compileShader(GL_FRAGMENT_SHADER, fragment)};

    unsigned int program{glCreateProgram()};

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::cout << "Failed to link program. " << std::endl;
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char info[length];
        glGetProgramInfoLog(program, length, &length, info);
        std::cout << info << std::endl;
    }

    return program;
}

static unsigned int compileShader(unsigned int type, const std::string &code) {
    unsigned int shader{glCreateShader(type)};
    auto src{code.c_str()};
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << std::endl;
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char info[length];
        glGetShaderInfoLog(shader, length, &length, info);
        std::cout << "MESSAGE: " << info << std::endl;
    }

    return shader;
}

static unsigned int linkShader(const std::string &vertexCode, const std::string &fragmentCode) {
    auto vertexShader{compileShader(GL_VERTEX_SHADER, vertexCode)};
    auto fragmentShader{compileShader(GL_FRAGMENT_SHADER, fragmentCode)};

    unsigned int program{glCreateProgram()};
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        std::cout << "Failed to link shader." << std::endl;
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char info[length];
        glGetProgramInfoLog(program, length, &length, info);
        std::cout << "MESSAGE: " << info << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}