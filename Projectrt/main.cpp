#include <raylib.h> //includes libararies
#include <iostream>
#include <conio.h>
#include <chrono>
#include <thread>
#include <cstdint>
#include <ctime>
#include <cstdlib>


#define SCREEN_WIDTH 1080 // defining screen height
#define SCREEN_HEIGHT 720 // defining screen width
#define CHARACTER_SPEED .5f // character speed
#define GOAL_LINE1 100 // goal line location
#define GOAL_LINE2 980 // goal 2 line location
#define MOVE_TIME 1000 // enemy moves in millsec

using std::cout; // define variables
Vector2 playerpos{SCREEN_WIDTH - 1060, SCREEN_HEIGHT / 2}; // player posistion
int Difficulty = -1;
int score = -10;

struct enemy //enemy items
{
	Vector2 position;
	Vector2 direction;
	static constexpr float enemySpeed = CHARACTER_SPEED / 3.5f; //const enemy speed 
	bool boundsCheck(); // check for out of bounds
	void BoundsResolution(); //action for aout of bounds

}enemies[15]{}; // array of enemies;

struct button // create button
{

	Rectangle rect;
	bool isMouseOver();
};

//union enemytype
enum struct screenType
{
	startScreen, gamePlayScreen, endScreen
};

screenType screenState = screenType::startScreen; //screen type


uint64_t moveClock = 0; // keeps track of time, for each enemy movmenemt change timmmer
void updateplayer(uint64_t);
void drawGoalLine();
void spawnpoints();
bool checkcolide();
void changeEnemyDirections();
void moveEnimeies(uint64_t);
void drawenemies();
void DrawCharacter();
void startGameLoop();
void MainLoop();
bool endGameLoop();




int main() //main function
{
	bool shouldExit = false;
	while (!shouldExit)
	{
		startGameLoop();
		MainLoop();
		shouldExit = endGameLoop();// end program
	}
	
}

void MainLoop() // main game loop
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Willson Run"); // screen paramiters
	SetWindowState(FLAG_VSYNC_HINT); //set window
	SetTargetFPS(60); //frame limit
	spawnpoints(); // enemy spawn
	srand(time(0)); //run time
	uint64_t delta = 0;

	while (!WindowShouldClose()) //main functuin
	{
		const auto start = std::chrono::steady_clock::now(); //set time and include all functions
		updateplayer(delta);
		if (checkcolide()) // gameover
		{
			CloseWindow();
			return;
		}
		changeEnemyDirections();
		moveEnimeies(delta);
		BeginDrawing();
		ClearBackground({ 0, 0, 0, 255 }); //background color
		drawenemies();
		DrawCharacter();  // draw player
		drawGoalLine();
		EndDrawing();


		DrawFPS(10, 10);
		const auto end = std::chrono::steady_clock::now(); // end time
		delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //use time start and fin to keep smooth playback
		moveClock += delta;


	}
}

[[nodiscard]]
int clamp(int min, int max, int toClamp) //filteres out variables not within paramaters
{
	if (toClamp <= min) 
		return min;
	if (toClamp >= max) 
		return max;
	return toClamp;
}

void startGameLoop() // title screen
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Willson Run Start"); // screen paramiters
	SetWindowState(FLAG_VSYNC_HINT); //set window
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
	int Key = GetKeyPressed();
		BeginDrawing();
		ClearBackground(DARKBLUE);
		DrawText("Press any button to Start", 200, 100, 50, PURPLE);
		DrawText("Press ESC to Exit", 300, 400, 50, PURPLE);
		SetExitKey(KEY_NULL);
		if (Key == KeyboardKey::KEY_ESCAPE) // close program
		{
			CloseWindow();
			exit(0);
		}

		if (Key) // if true or not zero
		{
			CloseWindow();
			return;
		}
		EndDrawing();
	}
}

bool endGameLoop() // game over screen
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Willson Run Game Over"); // screen paramiters
	SetWindowState(FLAG_VSYNC_HINT); //set window
	SetTargetFPS(60);
	button B1,B2; //creates button
	B1.rect = {.x = 150, .y = 400, .width = 350, .height = 50};
	B2.rect = {.x = 600, .y = 400, .width = 400, .height = 50};
	const char* SC = TextFormat("Score = %d", score);
	Color B1color = DARKPURPLE;
	Color B2color = DARKPURPLE;
	while (!WindowShouldClose()) // buttons 
	{
		BeginDrawing();
		ClearBackground(BLACK);
		if (B1.isMouseOver()) // close program
		{
			B1color = PURPLE;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				
				CloseWindow();
				return true;
			}

		}
		else B1color = DARKPURPLE;

		if (B2.isMouseOver()) // continue game
		{
			B2color = PURPLE;
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				CloseWindow();
				return false;
			}
		}
		else B2color = DARKPURPLE;

		DrawRectangle(600, 400, 400, 50, B2color);
		DrawRectangle(150, 400, 350, 50, B1color);
		DrawText("GAME OVER", 250, 200, 100, RED);
		DrawText(SC, 250, 300, 100, RED);
		DrawText("Press to Retry", 600, 400, 50, BLUE);
		DrawText("Press to Exit", 150, 400, 50, RED);
		EndDrawing();
	}
}

bool button::isMouseOver() // detects if mouse is over button
{
	Vector2 Mousep = GetMousePosition();
	return( Mousep.x >= rect.x && 
			Mousep.x <= (rect.x + rect.width) &&
			Mousep.y >= rect.y &&
			Mousep.y <= (rect.y + rect.height)
			);
}
void DrawCharacter()
{ 
	if (Difficulty >= 5)
		DrawCircle(playerpos.x, playerpos.y, 8, PINK);// draw player new dificulty
	else
		DrawCircle(playerpos.x, playerpos.y, 8, PURPLE);// draw player
}
void updateplayer(uint64_t delta)  //player controls
{
	if (IsKeyDown(KEY_A)) // move left
	{
		playerpos.x -= delta * CHARACTER_SPEED;
	}
	if (IsKeyDown(KEY_W)) // move up
	{
		playerpos.y -= delta * CHARACTER_SPEED;
	}
	if (IsKeyDown(KEY_S)) // move right
	{
		playerpos.y += delta * CHARACTER_SPEED;
	}
	if (IsKeyDown(KEY_D)) // move down
	{
		playerpos.x += delta * CHARACTER_SPEED;
	}
	playerpos.y = clamp(0, SCREEN_HEIGHT, playerpos.y); // keeps player on screen
	playerpos.x = clamp(0, SCREEN_WIDTH, playerpos.x); // keeps player on screen
}

void drawenemies() //enemy texture
{
	for (auto enemy : enemies) // for all enemies
	{
		if (Difficulty >= 5)
			DrawCircleV(enemy.position, 10.0f, PURPLE); //draw new color color
		
		else
			DrawCircleV(enemy.position, 10.0f, BLUE); //draw color
		
		
	}
}
void drawGoalLine() //goal
{
	const char* SC = TextFormat("Score = %d", score);
	static bool check = false; // checks status of goal

	if (playerpos.x < GOAL_LINE1 && check == false) // cross left line checkpoint
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, GREEN);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, RED);
		check = true;
		Difficulty += 1;
		score += 10;
		
	}
		else if (playerpos.x < GOAL_LINE1 && check == true) // if no checkpoint do nothing
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, GREEN);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, RED);
	}
	if (GOAL_LINE1 < playerpos.x < GOAL_LINE2 && check == true) //info between checkpoints
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, GREEN);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, RED);
	
	}
	if (GOAL_LINE1 < playerpos.x < GOAL_LINE2 && check == false) //info between checkpoints
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, RED);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, GREEN);

	}
	if (playerpos.x > GOAL_LINE2 && check == true) // cross right line checkpoint
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, RED);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, GREEN);
		check = false;
		Difficulty += 1;
		score += 10;
	}
		else if (playerpos.x > GOAL_LINE2 && check == false) // if no checkpoint do nothing
	{
		DrawRectangle(GOAL_LINE1, 0, 10, 1080, RED);
		DrawRectangle(GOAL_LINE2, 0, 10, 1080, GREEN);
	}
	
	if (Difficulty >= 5)
		for (auto enemy : enemies) // for all enemies
		{
			DrawCircleV(enemy.position, 10.0f, PURPLE); //draw new color color
		}
	DrawText(SC, 115, 10, 25, RED);
	
}


void changeEnemyDirections() //changes enemy movement
 {
 //make sure enough time has passed before we perform this function
	if (moveClock > MOVE_TIME)
		moveClock = 0;
	else return;
	
	for (auto& enemy : enemies) // for all enemies
	{
		auto rng = rand() % 4; // random number generator between 0-3
		
		if (rng < 2)//rand picked up or down (rng is either 0 or 1 here)
		{
			enemy.direction.y = rng ? 1 : -1;//map the y value to either -1 or 1
			enemy.direction.x = 0;//make it so the enemy wont move in the left and right direction also
		}
		else//rand picked left or right (rng is either 2 or 3 here)
		{
			enemy.direction.x = (rng == 2) ? -1 : 1;//map the x value to -1 or 1
			enemy.direction.y = 0;//make it so the enemy wont move in the up and down direction also
		}
		
	}
}


void moveEnimeies(uint64_t deltaTime) //moves enemies
{
	// scale the normalized direction vectors up turing into velocity vec.
	for (auto& enemy : enemies)
	{
		if (Difficulty >= 5) // increase movement speed on dificulty 
		{
			enemy.position.x += (enemy.direction.x * deltaTime * enemy::enemySpeed*1.5);// moves enemy in x direction
			enemy.position.y += (enemy.direction.y * deltaTime * enemy::enemySpeed*1.5);// moves enemy in y direction
		}
		else
		{
			enemy.position.x += (enemy.direction.x * deltaTime * enemy::enemySpeed);// moves enemy in x direction
			enemy.position.y += (enemy.direction.y * deltaTime * enemy::enemySpeed);// moves enemy in y direction
		}
			if (enemy.boundsCheck()) // check for out of bounds
			{
				enemy.BoundsResolution();// bounce them back
			}
		

	}
}

void spawnpoints() // initializes randomiazes enemy locations
{
	for (auto& enemy : enemies)// all enemies
	{
		enemy.position.x = GOAL_LINE1 + rand() % (GOAL_LINE2 - GOAL_LINE1); // prevents from spwaing in safe zone 
		enemy.position.y = rand() % SCREEN_HEIGHT; // limits hight for spawning
	}
}

bool areCirclesColidding(Vector2 p0, Vector2 p1, float r0, float r1) //if checks if objects collide
{
	Vector2 length;
	float lengthsqrd;
	float radiusSum;
	float radiusSquared;
	

	length.x = p1.x - p0.x; //distance from x values
	length.y = p1.y - p0.y; // distance from y values 
	lengthsqrd = (length.x * length.x) + (length.y * length.y); //x, y cordanates and their distance from each other
	radiusSum = r0 + r1; // circle radius
	radiusSquared = radiusSum * radiusSum; // the area of the circle

	return (lengthsqrd < radiusSquared);// detecs if distance is overlaping
}

bool enemy::boundsCheck() // checks for position in reguards to bounds
{
	return (position.x <= GOAL_LINE1 || position.x >= GOAL_LINE2 || position.y >= SCREEN_HEIGHT || position.y <= 0);
}

void enemy::BoundsResolution() // if on edge of bounds, reverese directions
{
	direction.x *= -1;
	direction.y *= -1;
}

bool checkcolide() //if collide function activates return the function has collided
{
	for (const auto enemy : enemies)
	{
		if (areCirclesColidding(enemy.position, playerpos, 10, 5))
			return true;
	}
	return false;
}