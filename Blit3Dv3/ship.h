#include "Blit3D.h"
#include "Asteroid.h"
#pragma once


class Shot 
{
public:
	glm::vec2 velocity, position;
	Sprite *sprite = NULL;
	float timeToLive = 2.f; //shots live for 2 seconds
	void Draw();
	bool Update(float seconds); //return false if shot dead (timeToLive <= 0 )

};

class Ship
{
public:
	Sprite *shotSprite;
	Sprite *invincibilitySprite;
	//Sprite *explosionSprite;

	std::vector<Sprite *>explosionSprite;
	std::vector<Sprite *>spriteList;
	glm::vec2 velocity, position;
	
	float angle = 0; //degrees
	float shotTimer = 0;
	float radius = 21.5f;
	float thrustTimer = 0;
	float invincibility = 3.f;
	float timerExplosion = 0.f;

	int lives = 3;
	int frameNumber = 0;

	void Draw();
	void Update(float seconds);
	void Thrust(float seconds);
	
	bool Shoot(std::vector<Shot>&shotList);
	bool thrusting = false;
	bool turningLeft = false;
	bool turningRight = false;
	
};

//colision function
bool Collide(Asteroid &A, const Ship &S);
bool Collide(Asteroid &A, const Shot &S);