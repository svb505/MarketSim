#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include "svbmath.h"
#include "bounds.h"

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
    Bounds bounds;
    float scale = 0.0f;

    bool LoadOBJ(const std::string& filename,float scale);
    bool LoadTexture(const std::string& filename);

    void CreateVBO();
    void Draw(svbmath::Vec3 pos, float yaw, float pitch);

    ~OBJModel();
};