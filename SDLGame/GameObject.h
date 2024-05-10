#include <stdio.h>
#include <cstdlib>
#include <SDL.h>

class GameObject 
{
public:
	//Constructor
	GameObject(const int w, const int h, int x, int y);
	//Deconstructor
	~GameObject();
	
	//Get ship vertex
	SDL_Vertex* getVertex();
	//Get projectiles

	SDL_Point* getProjectile();
	//Fire projectile
	void shootProjectile();

	//Spawn enemies
	void spawnEnemy();
	//Get enemies
	SDL_Rect* getEnemy();

	// Set new direction
	void setDirection(float dir);
	//Move
	void setMove();

	// Collision check for bullets
	void collisionCheck();
	// Collision check for ship
	void collisionShip();
	//Update ship
	void update();
private:
	int xpos; // Horizontal position of ship
	int ypos; // Vertical position of ship
	int width; // Width of screen
	int height; // Height of screen
	float radius; // Radius for ship
	float direction; // Direction of ship
	SDL_Vertex ship[3]; // Ship vertex
	SDL_Rect enemy[10]; // Enemies
	SDL_Point proj[10]; // Projectiles
	float projDir[10]; // Projectile direction
	float enemyDir[10]; // Enemy direction
};
