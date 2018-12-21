#include "Asteroid.h"

extern Sprite *largeAsteroidSprite;
extern Sprite *mediumAsteroidSprite;
extern Sprite *smallAsteroidSprite;

std::uniform_real_distribution<float>dist360(0.f, 360.f);
std::uniform_real_distribution<float>donutDistance(200.f, 1080/2.f);
std::uniform_real_distribution<float>distRotationSpeed(-30.f, 30.f);

void Asteroid::Draw()
{
	sprite->angle = angle;
	sprite->Blit(position.x, position.y);

	//wrap around
	sprite->angle = angle - 90;
	sprite->Blit(position.x, position.y);

	//redraw if too close to an edge
	//left
	if (position.x < radius/2)sprite->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius/2))sprite->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius/2)sprite->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius/2))sprite->Blit(position.x, position.y - 1080.f);


}

void Asteroid::Update(float seconds)
{
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	position += velocity * seconds;
	
	angle += rotationSpeed * seconds;
	if (angle > 360.f) angle -= 360.f;
	else if (angle < 360.f) angle += 360.f;

	
}

void MakeAsteroids(std::vector<Asteroid> &asteroidList, int level)
{
	asteroidList.clear();

	int numAsteroids = level + 2;

	float asteroidSpeed = 200.f + 20.f * level;

	for (int i = 0; i < numAsteroids; ++i)
	{
		//make a new large asteroids
		Asteroid a;

		a.angle = dist360(rng);

		//position the asteroid in a random area around the ship
		float randomAngle = glm::radians(dist360(rng));
		float distance = donutDistance(rng);

		a.position.x = cos(randomAngle) * distance;
		a.position.y = sin(randomAngle) * distance;

		a.radius = 405.f / 2.f;
		a.size = AsteroidEnum::LARGE;
		a.sprite = largeAsteroidSprite;
		a.rotationSpeed = distRotationSpeed(rng);

		randomAngle = glm::radians(dist360(rng));
		a.velocity.x = cos(randomAngle) * asteroidSpeed;	//1920.f + cos...
		a.velocity.y = sin(randomAngle) * asteroidSpeed;	//1080.f + sin...

		//add to list
		asteroidList.push_back(a);

	}
}