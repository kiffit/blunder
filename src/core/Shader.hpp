#pragma once
#include <glad/glad.h>
#include <string>

class Shader {
  public:
    GLuint id = 0;

    Shader() = default;
    ~Shader();

    bool loadGraphics(const std::string &vertPath, const std::string &fragPath);
    bool loadCompute(const std::string &computePath);
    bool reload();
    void use() const;

    // uniform helpers
    void setInt(const std::string &name, int v) const;
    void setFloat(const std::string &name, float v) const;
    void setVec2(const std::string &name, float x, float y) const;

  private:
    std::string vPath, fPath, cPath;
    bool isCompute = false;

    std::string readFile(const std::string &path);
    GLuint compile(GLenum type, const std::string &src);
};