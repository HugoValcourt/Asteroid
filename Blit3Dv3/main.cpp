/*
Simple example of loading/rotating/displaying sprites in Blit3D
*/
#include "Blit3D.h"
#include "ship.h"
#include "Asteroid.h"
#include "AudioEngine.h"

Blit3D *blit3D = NULL;

//memory leak detection
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#include "AudioEngine.h"

//GLOBAL DATA
double elapsedTime = 0;

float angle = 0; //for rotating the hearts
float timeSlice = 1.f / 120.f;
float timerForInstructionText = 5.f;
float timerForBipingSound = 0.5f;
float timerForTitleScreen = 3.f;
float instructionFlashing = 1.f;


int level = 1;
int rewardAt = 25000;
int i_score = 0;
int bigScore = 500;
int mediumScore = 250;
int smallScore = 100;

Ship * ship;
Shot * shot;

bool shoot = false;
bool giveLifes = false;
bool drumsPaused = false;
bool paused = false;
bool flashingText = false;

Sprite *backgroundSprite = NULL; //a pointer to a background sprite
Sprite *largeAsteroidSprite = NULL;
Sprite *mediumAsteroidSprite = NULL;
Sprite *smallAsteroidSprite = NULL;
Sprite *gamerOverSprite = NULL;
Sprite *titleSprite = NULL;
Sprite *pauseSprite = NULL;

std::vector<Shot> shotList;
std::vector<Asteroid>asteroidList;
std::mt19937 rng;

std::uniform_real_distribution<float>dist3602(0.f, 360.f);
std::uniform_real_distribution<float>distAsteroidAngle(-1.f, 1.f);
std::uniform_real_distribution<float>distRotationSpeed2(-30.f, 30.f);

enum GameState { Playing, GameOver, Title, Pause };
GameState state = Title;

AngelcodeFont *earthorbiter = NULL;

AudioEngine * audioE = NULL;
AkGameObjectID mainGameID = 1;
AkPlayingID musicID, shootingID, explosionID, lifeUpID,
levelUpID, thrustingID, gameOverID, asteroidExplotionID,
bipingID;

void Init()
{
	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.

	blit3D->ShowCursor(false);

	//load our image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\background.png");
	gamerOverSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\GameOverScreen.png");
	titleSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\TitleScreen.png");
	pauseSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\PauseScreen.png");

	largeAsteroidSprite = blit3D->MakeSprite(331, 69, 405, 387, "Media\\AsteroidSprite.png");
	mediumAsteroidSprite = blit3D->MakeSprite(130, 659, 206, 202, "Media\\AsteroidSprite.png");
	smallAsteroidSprite = blit3D->MakeSprite(725, 718, 78, 80, "Media\\AsteroidSprite.png");

	//load font
	earthorbiter = blit3D->MakeAngelcodeFontFromBinary32("earthorbiter_2.bin");

	//create ship
	ship = new Ship;
	//load invincibility bubble
	ship->invincibilitySprite = blit3D->MakeSprite(95, 99, 60, 60, "Media\\bubble.png");
	//load explosion

	for (int i = 0; i < 3; i++)
	{
		ship->explosionSprite.push_back(blit3D->MakeSprite(6, 4, 48, 51, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(67, 4, 53, 54, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(125, 4, 60, 55, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(189, 4, 60, 56, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(66, 71, 54, 51, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(131, 73, 52, 49, "Media\\exp2.png"));
		ship->explosionSprite.push_back(blit3D->MakeSprite(196, 74, 50, 47, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(6, 139, 47, 45, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(71, 140, 45, 43, "Media\\exp2.png"));
		ship->explosionSprite.push_back(blit3D->MakeSprite(139, 141, 40, 36, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(204, 142, 33, 34, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(14, 205, 33, 34, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(89, 206, 20, 26, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(154, 214, 4, 7, "Media\\exp2.png"));
		//ship->explosionSprite.push_back(blit3D->MakeSprite(219, 215, 3, 6, "Media\\exp2.png"));
	}


	for (int i = 0; i < 4; i++)
	{
		ship->spriteList.push_back(blit3D->MakeSprite(103, 102, 43, 59, "Media\\spaceship.png"));
		ship->spriteList.push_back(blit3D->MakeSprite(103, 102, 43, 59, "Media\\spaceship2.png"));
		ship->spriteList.push_back(blit3D->MakeSprite(103, 102, 43, 59, "Media\\spaceship3.png"));
		ship->spriteList.push_back(blit3D->MakeSprite(103, 102, 43, 59, "Media\\spaceship4.png"));
	}

	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);

	ship->shotSprite = blit3D->MakeSprite(0, 0, 10, 10, "Media\\shot.png");

	MakeAsteroids(asteroidList, level);

	//create audio engine
	audioE = new AudioEngine;
	audioE->Init();
	audioE->SetBasePath("Media\\");

	//load banks
	audioE->LoadBank("Init.bnk");
	audioE->LoadBank("MainBank.bnk");

	//register our game objects
	audioE->RegisterGameObject(mainGameID);

	//start playing the looping drums
	//We can play events by name:
	//drumsID = audioE->PlayEvent("Drums", mainGameID); //background music
	musicID = audioE->PlayEvent("BackgroundMusic", mainGameID); 
}

void DeInit(void)
{
	if (audioE != NULL) delete audioE;
	//any sprites still allocated are freed automatcally by the Blit3D object when we destroy it

	delete ship;

	asteroidList.clear();
}

void Update(double seconds)
{//must always update audio in our game loop
 
	audioE->ProcessAudio();

	switch (state)
	{
	case Playing:
	{
		if (seconds < 0.15)
			elapsedTime += seconds;

		else elapsedTime += 0.15;

		while (elapsedTime >= timeSlice)
		{
			elapsedTime -= timeSlice;
			ship->Update(timeSlice);

			//update the Wwise parameter for "Shipx",
			//which is mapped to pan it left/right
			float xpos = (ship->position.x - blit3D->screenWidth / 2) / (blit3D->screenWidth / 2);
			//AKRESULT result = AK::SoundEngine::SetRTPCValue(L"shipx", (AkRtpcValue)xpos, mainGameID);
			//assert(result == AK_Success);

			int shipLifes = ship->lives;



			if (ship->lives == 1 && timerForBipingSound <= 0)
			{
				bipingID = audioE->PlayEvent("Biping", mainGameID);

				timerForBipingSound = 0.5f;

			}

			AKRESULT result2 = AK::SoundEngine::SetRTPCValue(L"shiplifes", (AkRtpcValue)shipLifes, mainGameID);
			assert(result2 == AK_Success);

			if (shoot)
				if (ship->Shoot(shotList))
				{	shootingID = audioE->PlayEvent("Shooting", mainGameID);
				}

			for (int i = shotList.size() - 1; i >= 0; --i)
			{
				if (!shotList[i].Update(timeSlice))
					shotList.erase(shotList.begin() + i);
			}

			for (auto &A : asteroidList) A.Update(timeSlice);

			if (ship->invincibility <= 0)
			{
				for (int i = asteroidList.size() - 1; i >= 0; --i)
				{
					if (Collide(asteroidList[i], *ship))
					{
						explosionID = audioE->PlayEvent("Explosion", mainGameID);
						ship->lives--;
						ship->invincibility = 3.f;
						ship->timerExplosion = 1.f;
						if (ship->lives <= 0)
						{
							gameOverID = audioE->PlayEvent("GameOver", mainGameID);
							ship->lives = 0;
							state = GameState::GameOver;

						}

					}
				}

			}

			for (int i = asteroidList.size() - 1; i >= 0; --i)
			{
				for (int j = shotList.size() - 1; j >= 0; --j)
				{
					if (Collide(asteroidList[i], shotList[j]))
					{
						asteroidExplotionID = audioE->PlayEvent("AsteroidExplotion", mainGameID);

						shotList.erase(shotList.begin() + j);
						switch (asteroidList[i].size)
						{
						case AsteroidEnum::SMALL:
						{
							i_score += smallScore;

							asteroidList.erase(asteroidList.begin() + i);
						}
						break;
						case AsteroidEnum::MEDIUM:
						{
							i_score += mediumScore;

							Asteroid a;
							a.position = asteroidList[i].position;
							//how fast was the asteroid moving?
							float speed = glm::length(asteroidList[i].velocity);
							//scale up speed for faster, smaller asteroids

							speed *= 1.3f;

							//calculate what angle the asteroid has been drifting at
							float movementAngle = atan2f(asteroidList[i].velocity.y,
								asteroidList[i].velocity.x);

							//drift at new angles
							float newAngle = movementAngle + distAsteroidAngle(rng);
							a.velocity.x = cos(newAngle) * speed;
							a.velocity.y = sin(newAngle) * speed;

							newAngle = movementAngle + distAsteroidAngle(rng);
							asteroidList[i].velocity.x = cos(newAngle) * speed;
							asteroidList[i].velocity.y = sin(newAngle) * speed;

							//change sprite
							a.sprite = asteroidList[i].sprite = smallAsteroidSprite;

							a.radius = asteroidList[i].radius = 78.f / 2.f;
							a.size = asteroidList[i].size = AsteroidEnum::SMALL;

							a.angle = dist3602(rng);
							asteroidList[i].angle = dist3602(rng);

							a.rotationSpeed = distRotationSpeed2(rng);
							asteroidList[i].rotationSpeed = distRotationSpeed2(rng);

							asteroidList.push_back(a);
						}

						break;

						case AsteroidEnum::LARGE:
						{
							i_score += bigScore;

							Asteroid a;
							a.position = asteroidList[i].position;
							//how fast was the asteroid moving?
							float speed = glm::length(asteroidList[i].velocity);
							//scale up speed for faster, smaller asteroids
							speed *= 1.3f;

							//calculate what angle the asteroid has been drifting at
							float movementAngle = atan2f(asteroidList[i].velocity.y,
								asteroidList[i].velocity.x);

							//drift at new angles
							float newAngle = movementAngle + distAsteroidAngle(rng);
							a.velocity.x = cos(newAngle) * speed;
							a.velocity.y = sin(newAngle) * speed;

							newAngle = movementAngle + distAsteroidAngle(rng);
							asteroidList[i].velocity.x = cos(newAngle) * speed;
							asteroidList[i].velocity.y = sin(newAngle) * speed;

							//change sprite
							a.sprite = asteroidList[i].sprite = mediumAsteroidSprite;

							a.radius = asteroidList[i].radius = 206.f / 2.f;
							a.size = asteroidList[i].size = AsteroidEnum::MEDIUM;

							a.angle = dist3602(rng);
							asteroidList[i].angle = dist3602(rng);

							a.rotationSpeed = distRotationSpeed2(rng);
							asteroidList[i].rotationSpeed = distRotationSpeed2(rng);

							asteroidList.push_back(a);
						}
						break;
						}//end switch
						break;

					}

				}
			}

		}

		if (i_score == rewardAt && !giveLifes)
		{
			ship->lives++;

			lifeUpID = audioE->PlayEvent("LifeUp", mainGameID);

			rewardAt += 25000;
			giveLifes = true;
		}

		if (asteroidList.size() == 0)
		{
			level++;

			levelUpID = audioE->PlayEvent("LevelUp", mainGameID);

			ship->lives++;

			audioE->PlayEvent("LifeUp", mainGameID);

			MakeAsteroids(asteroidList, level);
		}
		ship->Update(static_cast<float>(seconds));

		if (timerForInstructionText > 0) timerForInstructionText -= seconds;
		if (timerForBipingSound > 0) timerForBipingSound -= seconds;



	}
	break;

	case GameOver:
	{

	}
	break;


	case Title:
	{
		if (timerForTitleScreen > 0) timerForTitleScreen -= seconds;

		if (instructionFlashing > 0) instructionFlashing -= seconds;

		if (timerForTitleScreen <= 0)
		{
			if (instructionFlashing <= 0)
			{
				instructionFlashing = 1.f;
				flashingText = !flashingText;

			}
		}


	}
	break;

	case Pause:
	{

	}
	default: Title;
		break;
	}



}

void Draw(void)
{
	switch (state)
	{
	case Playing:
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
												// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw stuff here

		//text next!
		std::string s_lives = "Lives: ";
		s_lives += std::to_string(ship->lives);

		std::string s_score = "Score: ";
		s_score += std::to_string(i_score);

		std::string s_level = "Level: ";
		s_level += std::to_string(level);

		std::string s_instruction = "Press 'M' to toggle music ON and OFF";



		//draw the background in the middle of the screen
		//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
		//starting as 0,0 in the bottom-left corner.
		backgroundSprite->Blit(1920.f / 2, 1080.f / 2);



		for (auto &S : shotList) S.Draw();

		for (auto &A : asteroidList) A.Draw();

		ship->Draw();

		earthorbiter->BlitText(15.f, 1060.f, s_score);
		earthorbiter->BlitText(15.f, 1010.f, s_lives);
		earthorbiter->BlitText(15.f, 960.f, s_level);
		if (timerForInstructionText > 0)
		{
			earthorbiter->BlitText(700.f, 50.f, s_instruction);
		}
	}
	break;

	case GameOver:
	{

		gamerOverSprite->Blit(1920.f / 2, 1080.f / 2);

	}
	break;

	case Title:
	{

		std::string s_titleInstruciton = "Press 'ENTER' to play";

		titleSprite->Blit(1920.f / 2, 1080.f / 2);

		if (flashingText)earthorbiter->BlitText(800.f, 250.f, s_titleInstruciton);

	}
	break;

	case Pause:
	{
		pauseSprite->Blit(1920.f / 2, 1080.f / 2);
	}

	default: Title;
		break;
	}

}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	switch (state)
	{
	case Playing:
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence

		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{

			drumsPaused = !drumsPaused;
			//We can pase/resume events:
			if(!drumsPaused)
				audioE->PauseEvent("BackgroundMusic", mainGameID, musicID);
			else
				audioE->ResumeEvent("BackgroundMusic", mainGameID, musicID);
		}

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence

		if (key == GLFW_KEY_A && action == GLFW_PRESS)
			ship->turningLeft = true;
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			ship->turningLeft = false;

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
			ship->turningRight = true;
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			ship->turningRight = false;

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			thrustingID = audioE->PlayEvent("Thrusting", mainGameID);

			ship->thrusting = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			audioE->StopEvent("Thrusting", mainGameID, thrustingID, 400);

			ship->thrusting = false;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			shoot = true;

		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
			shoot = false;

		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			level = 0;
			ship->lives = 2;
			asteroidList.clear();
		}

		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			paused = !paused;

			if (paused)state = Pause;

			if (!paused)state = Playing;
		}

	}
	break;

	case GameOver:
	{


		audioE->StopEvent("Thrusting", mainGameID, thrustingID);

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			state = GameState::Playing;
			i_score = 0;
			ship->Draw();
			level = 0;
			ship->lives = 2;
			asteroidList.clear();
		}

	}
	break;
	case Title:
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence

		if (timerForTitleScreen <= 0)
		{
			if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
				state = Playing;
		}
	}
	break;

	case Pause:
	{
		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			paused = !paused;
			state = Playing;
		}

	}

	default: Playing;
		break;
	}

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1920, 1080);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}