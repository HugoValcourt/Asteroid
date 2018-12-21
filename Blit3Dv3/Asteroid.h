#pragma once
#include <Blit3D.h>
#include <random>

extern std::mt19937 rng;

enum class AsteroidEnum {SMALL = 0, MEDIUM, LARGE};

class Asteroid
{
public:
	glm::vec2 velocity, position;
	Sprite *sprite = NULL;

	AsteroidEnum size = AsteroidEnum::LARGE;

	float radius = 0;
	float angle = 0; //sprite angle
	float rotationSpeed = 0; // tumble speed in degrees/seconds
	
	void Draw();
	void Update(float seconds);

	

};

void MakeAsteroids(std::vector<Asteroid> &asteroidList, int level);