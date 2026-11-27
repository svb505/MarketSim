struct Particle {
	float height = 55.0f;
	float x, y, z;
	float vx, vy, vz;
	float gravity = 9.8f;
	float fallSpeed;
};

class Weather {
private:
	const int diapazone = 160;
	bool isCleared = false;
	int count = 230;

	std::vector<Particle> particles = {};
public:
	Weather() { particles.reserve(count); }

	void draw();
	void generate(Camera& cam);
	void update(Camera& cam, double dt);
};