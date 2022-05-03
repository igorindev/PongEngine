#define PRESSED(b) (input->buttonsState[b].isDown)
#define CLICKED(b) (input->buttonsState[b].isDown && input->buttonsState[b].changed)
#define RELEASED(b) (!input->buttonsState[b].isDown && input->buttonsState[b].changed)

float playerPosition = 0.0f;
float playerVelocity;

float player2Position = 0.0f;
float player2Velocity;

float playerHalfSizeX = 2.5f;
float playerHalfSizeY = 12;

float arenaHalfSizeX = 85;
float arenaHalfSizeY = 45;

float ballPositionX;
float ballPositionY;
float ballVelocityX = 130;
float ballVelocityY = 0;

float ballHalfSize = 1;

int player1Score, player2Score;

enum class GameMode
{
	GM_MENU,
	GM_GAMEPLAY
};

GameMode currentGameMode;
bool hotButton;
bool enemyIsAI;

static bool AABBvsAABB(float p1x, float p1y, float hs1x, float hs1y, float p2x, float p2y, float hs2x, float hs2y)
{
	return (p1x + hs1x > p2x - hs2x &&
		p1x - hs1x < p2x + hs2x &&
		p1y + hs1y > p2y - hs2y &&
		p1y - hs1y < p2y + hs2y);
}

static void SimulatePlayer(float* position, float* velocity, float acceleration, float dt)
{
	acceleration -= *velocity * 10.0f;

	*position += *velocity * dt + acceleration * dt * dt * 0.5f;
	*velocity += acceleration * dt;

	//Collision
	if (*position + playerHalfSizeY > arenaHalfSizeY)
	{
		*position = arenaHalfSizeY - playerHalfSizeY;
		*velocity = 0;
	}
	else if (*position - playerHalfSizeY < -arenaHalfSizeY)
	{
		*position = -arenaHalfSizeY + playerHalfSizeY;
		*velocity = 0;
	}

}

static void SimulateGame(Input* input, float dt)
{
	DrawRect(0, 0, arenaHalfSizeX, arenaHalfSizeY, 0xffaa33);
	DrawArenaBorders(arenaHalfSizeX, arenaHalfSizeY, 0xff5500);

	if (currentGameMode == GameMode::GM_GAMEPLAY)
	{

		float acc = 2000; //Units per second

		//Player 1
		float playerAcceleration = 0.0f;
		if (PRESSED(BUTTON_UP)) { playerAcceleration += acc; }
		if (PRESSED(BUTTON_DOWN)) { playerAcceleration -= acc; }

		float player2Acceleration = 0.0f;
		if (!enemyIsAI)
		{		
			//Player 2
			if (PRESSED(BUTTON_W)) { player2Acceleration += acc; }
			if (PRESSED(BUTTON_S)) { player2Acceleration -= acc; }
		}
		else
		{
			//AI 1
			//if (ballPositionY > player2Position + 2.0f) player2Acceleration += 1300;
			//if (ballPositionY < player2Position - 2.0f) player2Acceleration -= 1300;

			//AI 2
			player2Acceleration = (ballPositionY - player2Position) * 100;
			if (player2Acceleration > 1300) player2Acceleration = 1300;
			if (player2Acceleration < -1300) player2Acceleration = -1300;
		}

		SimulatePlayer(&playerPosition, &playerVelocity, playerAcceleration, dt);
		SimulatePlayer(&player2Position, &player2Velocity, player2Acceleration, dt);

		//Simulate Ball
		{
			ballPositionX += ballVelocityX * dt;
			ballPositionY += ballVelocityY * dt;

			//Collision
			if (AABBvsAABB(ballPositionX, ballPositionY, ballHalfSize, ballHalfSize, 80, playerPosition, playerHalfSizeX, playerHalfSizeY))
			{
				ballPositionX = 80 - playerHalfSizeX - ballHalfSize;
				ballVelocityX *= -1;
				ballVelocityY = (ballPositionY - playerPosition) * 2 + playerVelocity * 0.75f;
			}
			else
				if (AABBvsAABB(ballPositionX, ballPositionY, ballHalfSize, ballHalfSize, -80, player2Position, playerHalfSizeX, playerHalfSizeY))
				{
					ballPositionX = -80 + playerHalfSizeX + ballHalfSize;
					ballVelocityX *= -1;
					ballVelocityY = (ballPositionY - player2Position) * 2 + player2Velocity * 0.75f;
				}

			if (ballPositionY + ballHalfSize > arenaHalfSizeY)
			{
				ballPositionY = arenaHalfSizeY - ballHalfSize;
				ballVelocityY *= -1;
			}
			else if (ballPositionY - ballHalfSize < -arenaHalfSizeY)
			{
				ballPositionY = -arenaHalfSizeY + ballHalfSize;
				ballVelocityY *= -1;
			}

			//Player 1 Goal
			if (ballPositionX + ballHalfSize > arenaHalfSizeX)
			{
				ballPositionY = ballPositionX = 0;
				ballVelocityX *= -1;
				ballVelocityY = 0;
				player1Score++;
			}
			else if (ballPositionX - ballHalfSize < -arenaHalfSizeX) //Player 2 Goal
			{
				ballPositionY = ballPositionX = 0;
				ballVelocityX *= -1;
				ballVelocityY = 0;
				player2Score++;
			}
		}

		DrawNumbers(player1Score, -10, 40, 1.0f, 0xbbffbb);
		DrawNumbers(player2Score, 10, 40, 1.0f, 0xbbffbb);

		//Rendering
		//Ball
		DrawRect(ballPositionX, ballPositionY, ballHalfSize, ballHalfSize, 0xffffff);
		//Player 1
		DrawRect(80, playerPosition, 2.5, 12, 0xff0000);
		//Player 2
		DrawRect(-80, player2Position, 2.5, 12, 0xff0000);
	}
	else
	{
		if (CLICKED(BUTTON_LEFT) || CLICKED(BUTTON_RIGHT)) {
			hotButton = !hotButton;
		}

		if (CLICKED(BUTTON_ENTER))
		{
			currentGameMode = GM_GAMEPLAY;
			enemyIsAI = hotButton ? 0 : 1;
		}

		if (!hotButton)
		{
			DrawAText("SINGLE PLAYER", -80, -10, 1, 0xff0000);
			DrawAText("MULTIPLAYER", 20, -10, 1, 0xaaaaaa);
		}
		else {
			DrawAText("SINGLE PLAYER", -80, -10, 1, 0xaaaaaa);
			DrawAText("MULTIPLAYER", 20, -10, 1, 0xff0000);
		}

		DrawAText("PONG ENGINE", -73, 40, 2, 0xffffff);
		DrawAText("WATCH THE STEP BY STEP TUTORIAL ON", -73, 22, .75, 0xffffff);
		DrawAText("YOUTUBE.COM/DANZAIDAN", -73, 15, 1.22, 0xffffff);
	}
}