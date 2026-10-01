#define NOMINMAX
#include "windows.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_image.h"
#include "svbmath.h"

bool OBJModel::LoadOBJ(const std::string& filename,float scale){
    this->scale = scale;

    std::ifstream file(filename);

    if (!file.is_open()){ 
        std::cout << "Cannot open: " << filename << std::endl;

        return false;
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texcoords;

    std::string line;

    while (std::getline(file, line)){
        std::stringstream ss(line);

        std::string type;
        ss >> type;

        // POSITION
        if (type == "v"){
            float x, y, z;

            ss >> x >> y >> z;

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        }
        // UV
        else if (type == "vt"){
            float u, v;

            ss >> u >> v;

            texcoords.push_back(u);
            texcoords.push_back(v);
        }
        // NORMAL
        else if (type == "vn"){
            float x, y, z;

            ss >> x >> y >> z;

            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);
        }
        // FACE
        else if (type == "f"){
            std::vector<std::string> face;
            std::string value;

            while (ss >> value) face.push_back(value);

            if (face.size() < 3) continue;

            auto ParseVertex = [&](const std::string& str) {
                Vertex v{};

                int p = 0;
                int t = 0;
                int n = 0;

                // -------------------------------------------------
                // v
                // -------------------------------------------------
                if (str.find('/') == std::string::npos) {

                    sscanf_s(str.c_str(), "%d", &p);

                    p--;

                    v.x = positions[p * 3 + 0];
                    v.y = positions[p * 3 + 1];
                    v.z = positions[p * 3 + 2];

                    v.u = 0.0f;
                    v.v = 0.0f;

                    // Нормаль пока отсутствует
                    v.nx = 0.0f;
                    v.ny = 1.0f;
                    v.nz = 0.0f;
                }

                // -------------------------------------------------
                // v//vn
                // -------------------------------------------------
                else if (str.find("//") != std::string::npos) {

                    sscanf_s(str.c_str(), "%d//%d", &p, &n);

                    p--;
                    n--;

                    v.x = positions[p * 3 + 0];
                    v.y = positions[p * 3 + 1];
                    v.z = positions[p * 3 + 2];

                    v.u = 0.0f;
                    v.v = 0.0f;

                    v.nx = normals[n * 3 + 0];
                    v.ny = normals[n * 3 + 1];
                    v.nz = normals[n * 3 + 2];
                }

                // -------------------------------------------------
                // v/vt/vn
                // -------------------------------------------------
                else {

                    sscanf_s(str.c_str(), "%d/%d/%d", &p, &t, &n);

                    p--;
                    t--;
                    n--;

                    v.x = positions[p * 3 + 0];
                    v.y = positions[p * 3 + 1];
                    v.z = positions[p * 3 + 2];

                    v.u = texcoords[t * 2 + 0];
                    v.v = texcoords[t * 2 + 1];

                    v.nx = normals[n * 3 + 0];
                    v.ny = normals[n * 3 + 1];
                    v.nz = normals[n * 3 + 2];
                }

                return v;
                };


            // TRIANGULATION
            // 1 2 3
            vertices.push_back(ParseVertex(face[0]));
            vertices.push_back(ParseVertex(face[1]));
            vertices.push_back(ParseVertex(face[2]));

            for (size_t i = 3; i < face.size(); i++){
                vertices.push_back(ParseVertex(face[0]));
                vertices.push_back(ParseVertex(face[i - 1]));
                vertices.push_back(ParseVertex(face[i]));
            }
        }
    }

    bounds.min = { vertices[0].x, vertices[0].y, vertices[0].z };
    bounds.max = { vertices[0].x, vertices[0].y, vertices[0].z };

    for (const auto& v : vertices) {
        bounds.min = { std::min(bounds.min.x, v.x),
                       std::min(bounds.min.y, v.y),
                       std::min(bounds.min.z, v.z) };

      
        bounds.max = { std::max(bounds.max.x, v.x),
                       std::max(bounds.max.y, v.y),
                       std::max(bounds.max.z, v.z)
        };
    }

    std::cout << "Model loaded\n" << "Vertices: " << vertices.size() << std::endl;

    return true;
}
void OBJModel::CreateVBO(){
    if (vertices.empty()) return;

    glGenBuffers(1,&vbo);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex),vertices.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void OBJModel::Draw(svbmath::Vec3 pos, float yaw, float pitch){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);   
    glRotatef(yaw, 0, 1, 0);
    glRotatef(pitch, 1, 0, 0);
    glScalef(scale, scale, scale);

    glBindTexture(GL_TEXTURE_2D,texture);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);


    // POSITION
    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3,GL_FLOAT,sizeof(Vertex),(void*)0);

    // NORMAL
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT,sizeof(Vertex),(void*)(3 * sizeof(float)));

    // UV
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2,GL_FLOAT,sizeof(Vertex),(void*)(6 * sizeof(float)));


    // DRAW
    glDrawArrays( GL_TRIANGLES, 0,vertices.size() );

    // DISABLE

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D,0);

    glPopMatrix();
};
OBJModel::~OBJModel(){
    if (vbo) glDeleteBuffers(1,&vbo);
    if (texture)glDeleteTextures(1,&texture);
}
bool OBJModel::LoadTexture(const std::string& filename){
    int width;
    int height;
    int channels;

    unsigned char* data = stbi_load( filename.c_str(), &width,&height, &channels,4);

    if (!data){
        std::cout << "Cannot load texture: " << filename<< std::endl;

        return false;
    }

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D,texture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return true;
}