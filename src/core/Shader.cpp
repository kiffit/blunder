#include "Shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Shader::~Shader() {
    if (id)
        glDeleteProgram(id);
}

std::string Shader::readFile(const std::string &path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::compile(GLenum type, const std::string &src) {
    GLuint s = glCreateShader(type);
    const char *cstr = src.c_str();
    glShaderSource(s, 1, &cstr, nullptr);
    glCompileShader(s);

    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetShaderInfoLog(s, 1024, nullptr, log);
        std::cout << "Shader compile error:\n" << log << "\n";
        glDeleteShader(s);
        return 0;
    }

    return s;
}

bool Shader::loadGraphics(const std::string &vertPath, const std::string &fragPath) {
    vPath = vertPath;
    fPath = fragPath;
    cPath.clear();
    isCompute = false;

    std::string vSrc = readFile(vertPath);
    std::string fSrc = readFile(fragPath);

    GLuint vs = compile(GL_VERTEX_SHADER, vSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fSrc);

    if (!vs || !fs)
        return false;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cout << "Program link error:\n" << log << "\n";
        glDeleteProgram(prog);
        return false;
    }

    if (id)
        glDeleteProgram(id);
    id = prog;

    return true;
}

bool Shader::loadCompute(const std::string &computePath) {
    cPath = computePath;
    vPath.clear();
    fPath.clear();
    isCompute = true;

    std::string cSrc = readFile(computePath);
    GLuint cs = compile(GL_COMPUTE_SHADER, cSrc);
    if (!cs)
        return false;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, cs);
    glLinkProgram(prog);

    glDeleteShader(cs);

    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (!success) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cout << "Compute link error:\n" << log << "\n";
        glDeleteProgram(prog);
        return false;
    }

    if (id)
        glDeleteProgram(id);
    id = prog;

    return true;
}

bool Shader::reload() {
    if (isCompute)
        return loadCompute(cPath);
    else
        return loadGraphics(vPath, fPath);
}

void Shader::use() const { glUseProgram(id); }

void Shader::setInt(const std::string &name, int v) const { glUniform1i(glGetUniformLocation(id, name.c_str()), v); }

void Shader::setFloat(const std::string &name, float v) const { glUniform1f(glGetUniformLocation(id, name.c_str()), v); }

void Shader::setVec2(const std::string &name, float x, float y) const { glUniform2f(glGetUniformLocation(id, name.c_str()), x, y); }