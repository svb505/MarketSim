#define NOMINMAX
#include <windows.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "weather.h"
#include "camera.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include "svbmath.h"
#include "building.h"
#include "bounds.h"

void Weather::draw() {
	for (auto& p : particles) {
		glPushMatrix();
		glTranslatef(p.x, p.y, p.z);

		glColor3f(1.0f, 1.0f, 1.0f);

		glPointSize(3.0f);
		glBegin(GL_POINTS);

		glVertex3f(0.0f, 0.0f, 0.0f);

		glEnd();
		glPopMatrix();
	}
}
void Weather::generate(Camera& cam) {
	int toGenerate = std::min<int>(count - static_cast<int>(particles.size()), count);

	for (int i = 0; i < toGenerate; i++) {
		svbmath::Vec3 camPos = cam.getCamPos();

		Particle p;

		p.x = camPos.x + (rand() % int(diapazone * 2)) - diapazone;
		p.y = camPos.y + p.height;
		p.z = camPos.z + (rand() % int(diapazone * 2)) - diapazone;

		p.vx = 0.0f; p.vz = 0.0f;
		p.fallSpeed = 20.0f * svbmath::randFloat(0.5f, 1.0f);

		p.vy = -p.fallSpeed;

		particles.push_back(p);
	}
}
void Weather::update(Camera& cam, BuildingSettings& build, double dt) {
	for (auto& p : particles) {
		p.vy -= p.gravity * dt;
		p.y += p.vy * dt;
		
		svbmath::Vec3 pos = { p.x, p.y, p.z };

		if (p.y <= 0.0f || checkAABBCollision({build.min,build.max},pos, true)) {
			p.y = cam.getCamPos().y + p.height;
			p.vy = -p.fallSpeed;
		}
	}
}