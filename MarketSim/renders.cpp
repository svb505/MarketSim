#include "renders.h"
#include "texture.h"
#include <glad/glad.h>
#include "svbmath.h"
#include "svbRandom.h"

void drawBox(float width, float height, float depth, svbmath::Vec3 pos) {
    GLuint texture = (returnRandomNum(0.0f, 1.0f) < 0.5) ? allTextures["BOX1"] : allTextures["BOX2"];
    
    width /= 2.0f;
    height /= 2.0f;
    depth /= 2.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    glBegin(GL_QUADS);

    // Front (+Z)
    glNormal3f(0, 0, 1);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-width, -height, depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, -height, depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, height, depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-width, height, depth);


    // Back (-Z)
    glNormal3f(0, 0, -1);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(width, -height, -depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-width, -height, -depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-width, height, -depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(width, height, -depth);


    // Left (-X)
    glNormal3f(-1, 0, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-width, -height, -depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-width, -height, depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-width, height, depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-width, height, -depth);


    // Right (+X)
    glNormal3f(1, 0, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(width, -height, depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, -height, -depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, height, -depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(width, height, depth);


    // Top
    glNormal3f(0, 1, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-width, height, depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, height, depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, height, -depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-width, height, -depth);


    // Bottom
    glNormal3f(0, -1, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-width, -height, -depth);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(width, -height, -depth);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(width, -height, depth);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-width, -height, depth);

    glEnd();

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}