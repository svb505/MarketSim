#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "svbmath.h"

struct Vertex{
    float x, y, z;       // position
    float nx, ny, nz;    // normal
    float u, v;          // UV
};

class OBJModel{
private:
    std::vector<Vertex> vertices;

    GLuint vbo = 0;
    GLuint texture = 0;

public:
    bool LoadOBJ(const std::string& filename);
    bool LoadTexture(const std::string& filename);

    void CreateVBO();
    void Draw(svbmath::Vec3 pos, float yaw, float pitch);

    ~OBJModel();
};