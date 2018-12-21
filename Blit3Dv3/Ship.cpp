#include "ship.h"
#include "Asteroid.h"
#include <random>

void Shot::Draw()
{
	sprite -> Blit(position.x, position.y);
}

bool Shot::Update(float seconds)
{	
	//wrap around (took from the ship wrap around
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;
	
	position += velocity * seconds;

	if (timeToLive > 0) timeToLive -= seconds;
	if (timeToLive > 0) return true;

	return false;
	
}

void Ship::Draw()
{
	spriteList[frameNumber]->angle = angle - 90;
	spriteList[frameNumber]->Blit(position.x, position.y);

	//only if invincibility is greater than 0
	if (invincibility > 0 && lives > 0) 
	{
		invincibilitySprite->Blit(position.x, position.y);
	}
	if (timerExplosion > 0)
	{
		explosionSprite[frameNumber]->Blit(position.x, position.y);
	}

	//redraw if too close to an edge
	//left
	if (position.x < radius + 10.f)
	{
		spriteList[frameNumber]->Blit(position.x + 1920.f, position.y);

		//draws a bubble arround the ship if invincibility is on
		if (invincibility > 0) 
		{
			invincibilitySprite->Blit(position.x + 1920.f, position.y);
		}
		if (timerExplosion > 0)
		{
			explosionSprite[frameNumber]->Blit(position.x + 1920.f, position.y);
		}
	}
	//right
	if (position.x > 1920.f - (radius + 10.f)) 
	{
		spriteList[frameNumber]->Blit(position.x - 1920.f, position.y);
		//draws a bubble arround the ship if invincibility is on
		if (invincibility > 0)
		{
			invincibilitySprite->Blit(position.x - 1920.f, position.y);
		}
		if (timerExplosion > 0)
		{
			explosionSprite[frameNumber]->Blit(position.x - 1920.f, position.y);
		}
	}
	//down
	if (position.y < radius + 10.f)
	{
		spriteList[frameNumber]->Blit(position.x, position.y + 1080.f);
		//draws a bubble arround the ship if invincibility is on
		if (invincibility > 0)
		{
			invincibilitySprite->Blit(position.x, position.y + 1080.f);
		}
		if (timerExplosion > 0)
		{
			explosionSprite[frameNumber]->Blit(position.x, position.y + 1080.f);
		}
	}
	//up
	if (position.y > 1080.f - (radius + 10.f))
	{
		spriteList[frameNumber]->Blit(position.x, position.y - 1080.f);
		//draws a bubble arround the ship if invincibility is on
		if (invincibility > 0)
		{
			invincibilitySprite->Blit(position.x, position.y - 1080.f);
		}
		if (timerExplosion > 0)
		{
			explosionSprite[frameNumber]->Blit(position.x, position.y - 1080.f);
		}
	}
}

void Ship::Update(float seconds)
{
	//turning
	if (turningLeft)
	{
		angle += 180.f * seconds;
	}

	if (turningRight)
	{
		angle -= 180.f * seconds;
	}

	if (thrusting)
	{
		Thrust(seconds);
	}
	else frameNumber = 0;

	//update position
	position += velocity * seconds;

	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	if (scale < 0) scale = 0;
	velocity *= scale;
	
	//handle shot timer
	if (shotTimer > 0) shotTimer -= seconds;

	//invincibility timer
	if (invincibility > 0) invincibility -= seconds;

	if (timerExplosion > 0) timerExplosion -= seconds;
 	
}

bool Ship::Shoot(std::vector<Shot>&shotList)
{
	if (shotTimer > 0) return false;
	//time to shoot 
	//reset timer
	shotTimer = 1.f / 2.f;

	//make a new shot
	Shot shot;
	shot.sprite = shotSprite;
	
	glm::vec2 offset;
	float angleInRotation = glm::radians(angle);
	offset.x = cos(angleInRotation);
	offset.y = sin(angleInRotation);
	shot.position = position + offset * 35.f;

	

	//Build a vector from the ship angle
	shot.velocity.x = cos(glm::radians(angle));
	shot.velocity.y = sin(glm::radians(angle));

	//scale up the shot velocity
	shot.velocity *= 700.f;
	//add the ship velocity
	shot.velocity += velocity;

	//add shot to the shotList
	shotList.push_back(shot);

	//wing shots
	{
		Shot shot;
		shot.sprite = shotSprite;

		glm::vec2 offset;
		float angleInRotation = glm::radians(angle + 100);
		offset.x = cos(angleInRotation);
		offset.y = sin(angleInRotation);
		shot.position = position + offset * 20.f;



		//Build a vector from the ship angle
		shot.velocity.x = cos(glm::radians(angle));
		shot.velocity.y = sin(glm::radians(angle));

		//scale up the shot velocity
		shot.velocity *= 700.f;
		//add the ship velocity
		shot.velocity += velocity;
		
		shotList.push_back(shot);
	}

	{
		Shot shot;
		shot.sprite = shotSprite;

		glm::vec2 offset;
		float angleInRotation = glm::radians(angle - 100);
		offset.x = cos(angleInRotation);
		offset.y = sin(angleInRotation);
		shot.position = position + offset * 20.f;



		//Build a vector from the ship angle
		shot.velocity.x = cos(glm::radians(angle));
		shot.velocity.y = sin(glm::radians(angle));

		//scale up the shot velocity
		shot.velocity *= 700.f;
		//add the ship velocity
		shot.velocity += velocity;

		shotList.push_back(shot);
	}
	
	return true;
}

void Ship::Thrust(float seconds)
{
	//calculate facing vector
	float radians = angle * (M_PI / 180.f);
	glm::vec2 facing;
	facing.x = std::cos(radians);
	facing.y = std::sin(radians);

	facing *= seconds * 200.f;

	velocity += facing;

	//check if over mac speed
	if (velocity.length() > 600.f)
	{
		velocity = glm::normalize(velocity) * 600.f;
	}

	//animation timing
	if (thrustTimer < 1.f / 20.f)
		thrustTimer += seconds;

	else
	{
		//change frames
		frameNumber++;
		if (frameNumber > 3)
			frameNumber = 1;
		thrustTimer -= 1.f / 20.f;
	}
}

bool Collide(Asteroid &A, const Ship &S)
{
	glm::vec2 wrapPos;
	float distance;

	for (float xadd = -1920; xadd <= 1920; xadd += 1920)
		for (float yadd = -1080; yadd <= 1080; yadd += 1080)
		{
			float distance = glm::distance(A.position, S.position);
			wrapPos = A.position;
			wrapPos.x += xadd;
			wrapPos.y += yadd;
			distance = glm::distance(wrapPos, S.position);
			if (distance <= A.radius + S.radius) return true;
	}

	return false;
}

bool Collide(Asteroid &A, const Shot &S)
{

	glm::vec2 wrapPos;
	float distance;

	for (float xadd = -1920; xadd <= 1920; xadd += 1920)
		for (float yadd = -1080; yadd <= 1080; yadd += 1080)
		{
			float distance = glm::distance(A.position, S.position);
			wrapPos = A.position;
			wrapPos.x += xadd;
			wrapPos.y += yadd;
			distance = glm::distance(wrapPos, S.position);
			if (distance <= A.radius) return true;

		}

	return false;

}
