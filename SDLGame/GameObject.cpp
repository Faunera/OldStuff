#include "GameObject.h"

GameObject::GameObject(const int w, const int h, int x, int y) : width(w), height(h), xpos(x), ypos(y)
{	
	radius = 10;
	direction = 0;
	// Create a triangular gameship
	
	ship[0].position.x = xpos + cos(direction) * radius;
	ship[0].position.y = ypos + sin(direction) * radius;
	ship[0].color.r = 255;
	ship[0].color.g = 0;
	ship[0].color.b = 255;
	ship[0].color.a = 255;

	ship[1].position.x = xpos + cos((direction + 2.3 * M_PI / 3)) * radius;
	ship[1].position.y = ypos + sin(direction + 2.3 * M_PI / 3) * radius;
	ship[1].color.r = 255;
	ship[1].color.g = 0;
	ship[1].color.b = 255;
	ship[1].color.a = 255;

	ship[2].position.x = xpos + cos(direction - 2.3 * M_PI / 3) * radius;
	ship[2].position.y = ypos + sin(direction - 2.3 * M_PI / 3) * radius;
	ship[2].color.r = 255;
	ship[2].color.g = 0;
	ship[2].color.b = 255;
	ship[2].color.a = 255;

}

GameObject::~GameObject()
{
	delete ship;
	delete enemy;
}

SDL_Vertex* GameObject::getVertex()
{
	return ship;
}

void GameObject::shootProjectile()
{
	if (proj[9].x < 0) 
	{
		for (int i = 9; i > 0; --i) 
		{
			proj[i] = proj[i - 1];
			projDir[i] = projDir[i - 1];

			proj[i].x = xpos;
			proj[i].y = ypos;
			projDir[i] = direction;
		}	
	}
}

SDL_Point* GameObject::getProjectile()
{
	return proj;
}

void GameObject::spawnEnemy()
{
	for (int i = 0; i < 10; ++i) 
	{	
		if (enemy[i].x <= 0 || enemy[i].y <= 0) 
		{	
			// Two methods to make field a bit more full
			if (i < 5) 
			{
				enemy[i].x = rand() % (width / 2);
				enemy[i].y = rand() % (height / 2);
			}
			else 
			{
				enemy[i].x = rand() % (width /2) + width / 2;
				enemy[i].y = rand() % (height / 2) + height/ 2;
			}
		}
		enemy[i].w = 15;
		enemy[i].h = 15;
		enemyDir[i] = float(rand() / float(RAND_MAX) *  5);
	}
}

SDL_Rect* GameObject::getEnemy() 
{
	return enemy;
}

void GameObject::setDirection(float dir)
{
	// Multiply dir to make turning smoother
	direction +=  3 * dir;	
}

void GameObject::setMove() 
{
	xpos += cos(direction) * radius / 2;
	ypos += sin(direction) * radius / 2;
}

void GameObject::collisionCheck() 
{
	// Bullet Collision
	for (int i = 0; i < 10; ++i) 
	{
		for (int j = 0; j < 10; ++j) 
		{
			// Hit enemy
			if (SDL_PointInRect(&proj[i], &enemy[j]))
			{
				proj[i].x = -10;
				proj[i].y = -10;
				projDir[i] = 0;

				enemy[j].x = -10;
				enemy[j].y = -10;
				enemyDir[j] = 0;
			}	
		}
	}
}

void GameObject::collisionShip() 
{
	// Horizontal wall collision
	if (ship->position.x <= 1 || ship->position.x >= width) 
	{
		xpos = xpos - cos(direction) * radius;
	}

	// Vertical wall collision
	if (ship->position.y <= 1 || ship->position.y >= height)
	{
		ypos = ypos - sin(direction) * radius;
	}
	// Collision with enemy
	for (int i = 0; i < 10; ++i)
	{
		static SDL_Rect ships;
		ships.x = xpos - 5;
		ships.y = ypos - 5;
		ships.h = 10;
		ships.w = 10;
		if (SDL_HasIntersection(&ships, &enemy[i]))
		{
			printf("hit ship\n");
			enemy[i].x = -10;
			enemy[i].y = -10;
			enemyDir[i] = 0;

			xpos -= cos(direction) * radius;
			ypos -= sin(direction) * radius;
		}
		// Enemy hitting another enemy
		if (SDL_HasIntersection(&enemy[i], &enemy[i + 1]))
		{
			printf("Hit enemy +1\n");
			enemy[i].x -= cos(enemyDir[i]);
			enemy[i].y -= cos(enemyDir[i]);
			enemyDir[i] = -enemyDir[i];

			enemy[i + 1].x -= cos(enemyDir[i + 1]);
			enemy[i + 1].y -= cos(enemyDir[i + 1]);
			enemyDir[i + 1] = -enemyDir[i + 1];
		}
		
	}
}

void GameObject::update()
{
	// Update ship positions
	ship[0].position.x = xpos + cos(direction) * radius;
	ship[0].position.y = ypos + sin(direction) * radius;

	ship[1].position.x = xpos + cos(direction + 2.3 * M_PI / 3) * radius;
	ship[1].position.y = ypos + sin(direction + 2.3 * M_PI / 3) * radius;

	ship[2].position.x = xpos + cos(direction - 2.3 * M_PI / 3) * radius;
	ship[2].position.y = ypos + sin(direction - 2.3 * M_PI / 3) * radius;
	

	// Enemy update
	for (int i = 0; i < 10; ++i) 
	{
		// Shaky
		enemy[i].x += cos(enemyDir[i]) * M_PI;
		enemy[i].y += sin(enemyDir[i]) * M_PI;
		if (enemy[i].x < 0 || enemy[i].x > width || enemy[i].y < 0 || enemy[i].y > height) 
		{
			enemy[i].x = -10;
			enemy[i].y = -10;
			enemyDir[i] = 0;		
		}
	}

	// Update projectile positions
	for (int i = 0; i < 10; ++i)
	{
		proj[i].x += cos(projDir[i]) * 10;
		proj[i].y += sin(projDir[i]) * 10;
		if (proj[i].x < 0 || proj[i].x > width || proj[i].y < 0 || proj[i].y > height)
		{
			proj[i].x = -10;
			proj[i].y = -10;
			projDir[i] = 0;
		}
	}
	// Collisions
	collisionShip();
	collisionCheck();

	// Spawn more enemies
	spawnEnemy();
}
