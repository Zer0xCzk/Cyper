#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "object.h"
#include <math.h>
#include "bullet.h"


void Update(float dt);
void RenderFrame(float dt);

#define WW 1200
#define WH 900

SDL_Texture* TEXgun;

//=============================================================================

int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Cyper", WW, WH))
	{
		return 1;
	}

	TEXgun = IMG_LoadTexture(gRenderer, "assets/proto.png");

	StartLoop(Update, RenderFrame);

	return 0;
}



bool jump = true;
bool gen = false;
bool wallcling = false;
bool stomp = false;
const int ammount = 30;
Object player = { 0, 600, 50, 50, 800, 0};
Object pastplayer = player;
Object terrain[ammount] = { 0, 0, 0, 0, 0, 0};
Bullet bullet[50] = {0, 0, 0, 30, 10, 100, 0};
double bulletCooldown = 0;
unsigned int level = 0;
int horChunk = 0, verChunk = 0;
int deltaX, deltaY, mouseX, mouseY;
double result;
Object gun = {0, 0, 50, 20, 0, 0};
SDL_Point playerCenter = {0, 0};
SDL_Point rotationCenter = {-20 - player.box.w / 2, gun.box.h / 2};

//=============================================================================

void TerGen()
{
	/*
	Automatic terrain generation
	Keeping it in case I need to test stuff
	int spacing = 200;
	int xoffset = -50;
	int yoffset = 75;
	for (int i = 0; i < ammount; i++)
	{
		if (i % 2 == 0)
		{
			terrain[i].box.w = 450;
			terrain[i].box.h = 200;
			terrain[i].box.x = xoffset;
			terrain[i].box.y = yoffset + (i * spacing);
		}
		else
		{
			terrain[i].box.w = 450;
			terrain[i].box.h = 100;
			terrain[i].box.x = WW - xoffset - terrain[i].box.w;
			terrain[i].box.y = yoffset + (i * spacing);
		}
	}
	gen = true;
	*/
	switch (level) {
	// LEVEL 0: Tutorial
	case 0:
		//L0Start
		terrain[0].box.x = 0;
		terrain[0].box.w = WW / 4;
		terrain[0].box.y = WH - 200;
		terrain[0].box.h = 200;
		//L0Main
		terrain[1].box.x = terrain[0].box.w + 100;
		terrain[1].box.w = WW/2;
		terrain[1].box.y = terrain[0].box.y - 100;
		terrain[1].box.h = 50;
		
		terrain[2].box.x = terrain[1].box.x + terrain[1].box.w;
		terrain[2].box.w = 50;
		terrain[2].box.y = terrain[1].box.y - 2000;
		terrain[2].box.h = 2000;
		
		terrain[3].box.x = terrain[2].box.x - 400;
		terrain[3].box.w = 50;
		terrain[3].box.y = terrain[2].box.y - 100;
		terrain[3].box.h = 2000;


	}

	gen = true;
}

void newProjectile() 
{
	for(int i = 0; i <= 50; i++) {
		if( bullet[i].state != 1 && bulletCooldown == 0) {
			bullet[i].state = 1;
			bullet[i].angle = result;
			bullet[i].box.x = player.box.x;
			bullet[i].box.y = player.box.y;
			bulletCooldown = 60;
			return;
		}
		else {
			
		}
	}
}

void PosUp(float dt)
{
	//Gun rotation
	//TODO: Better description of what the fuck is happening
	gun.box.x = player.box.x + player.box.w + 20;
	gun.box.y = player.box.y + player.box.h / 2 - gun.box.h / 2;
	playerCenter.x = player.box.x + (player.box.w / 2);
	playerCenter.y = player.box.y + (player.box.h / 2);
	SDL_GetMouseState(&mouseX, &mouseY);
   	deltaX = mouseX - playerCenter.x;
	deltaY = mouseY - playerCenter.y;
   	result = (atan2(deltaY, deltaX) * 180.00000) / M_PI;
	rotationCenter = {-20 - player.box.w / 2, gun.box.h / 2};
	
	//Movement
	if (IsKeyDown(SDL_SCANCODE_A) && !stomp) {
		player.box.x -= (int)(player.speed * dt + 0.5f);
		if (player.box.x != pastplayer.box.x) {
			wallcling = false;
		}
	}
	if (IsKeyDown(SDL_SCANCODE_W) && jump && !wallcling) {
		player.vely = -800;
		jump = false;
		wallcling = false;
		stomp = false;
	}
	if (IsKeyDown(SDL_SCANCODE_D) && !stomp) {
		player.box.x += (int)(player.speed * dt + 0.5f);
		if (player.box.x != pastplayer.box.x) {
			wallcling = false;
		}
	}
	if (IsKeyDown(SDL_SCANCODE_S) && !wallcling) {
		player.vely = 2000;
		stomp = true;
		jump = true;
	}
	if (IsKeyDown(SDL_SCANCODE_SPACE)) {
		newProjectile();
	}
	//Check if we're clinging to a wall
	if (!wallcling) {
		player.vely += 20;
		player.box.y += (int)(player.vely * dt + 0.5f);
	}
	if (wallcling) {
		player.vely = 0;
	}
	//TODO: Make it U->D->L->R
	//Right transition
	if (player.box.x + player.box.w > WW) {
		while (player.box.x > 0) {
			player.box.x -= 5;
			for (int i = 0; i < ammount; i++) {
				terrain[i].box.x -= 5;
			}
		}
		horChunk++;
	}
	//Down transition
	if (player.box.y + player.box.h > WH) {
		while (player.box.y > 0)
		{
			player.box.y -= 5;
			for (int i = 0; i < ammount; i++)
			{
				terrain[i].box.y -= 5;
			}
		}
		verChunk++;
	}
	//Up transition
	if (player.box.y < 0) {
		while (player.box.y + player.box.h < WH)
		{
			player.box.y += 5;
			for (int i = 0; i < ammount; i++)
			{
				terrain[i].box.y += 5;
			}
		}
		verChunk--;
	}
	//Left transition
	if (player.box.x < 0) {
		while (player.box.x + player.box.w < WW)
		{
			player.box.x += 5;
			for (int i = 0; i < ammount; i++)
			{
				terrain[i].box.x += 5;
			}
		}
		horChunk--;
	}
	//Moving bullets
	for(int i = 0; i <=0; i++) {
		if (bullet[i].state == 0) break;
		else {
			//First Quadrant
			if(bullet[i].angle > 0 && bullet[i].angle <= 90) {
				double angleB = 360 - bullet[i].angle;
				bullet[i].box.x += (int)((bullet[i].speed * sin(bullet[i].angle)) * sin(90));
				bullet[i].box.y -= (int)((bullet[i].speed * sin(angleB)) * sin(90));
			}
			//Second Quadrant
			if(bullet[i].angle > 90 && bullet[i].angle <= 180) {
				double angleB = 360 - bullet[i].angle;
				bullet[i].box.x -= (int)((bullet[i].speed * sin(bullet[i].angle)) * sin(90));
				bullet[i].box.y -= (int)((bullet[i].speed * sin(angleB)) * sin(90));
			}
			//Third Quadrant
			if(bullet[i].angle > 180 && bullet[i].angle <= 270) {
				double angleB = 360 - bullet[i].angle;
				bullet[i].box.x -= (int)((bullet[i].speed * sin(bullet[i].angle)) * sin(90));
				bullet[i].box.y += (int)((bullet[i].speed * sin(angleB)) * sin(90));
			}
			//Fourth Quadrant
			if(bullet[i].angle > 270 && bullet[i].angle < 360) {
				double angleB = 360 - bullet[i].angle;
				bullet[i].box.x -= (int)((bullet[i].speed * sin(bullet[i].angle)) * sin(90));
				bullet[i].box.y -= (int)((bullet[i].speed * sin(angleB)) * sin(90));
			}
		}
	}
}

void ColUp(float dt)
{
	SDL_Point left_top = { player.box.x, player.box.y};
	SDL_Point right_top = { player.box.x + player.box.w, player.box.y };
	SDL_Point left_bottom = { player.box.x, player.box.y + player.box.h };
	SDL_Point right_bottom = { player.box.x + player.box.w, player.box.y + player.box.h };
	for (int i = 0; i < ammount; i++)
	{
		//Keeps the player above a rectangle
		if ((SDL_PointInRect(&right_bottom, &terrain[i].box) || SDL_PointInRect(&left_bottom, &terrain[i].box)) && pastplayer.box.y + pastplayer.box.h <= terrain[i].box.y)
		{
			player.box.y = terrain[i].box.y  - player.box.h;
			player.vely = 0;
			jump = true;
			stomp = false;
		}
		//Keeps the player below a rectangle
		else if ((SDL_PointInRect(&right_top, &terrain[i].box) || SDL_PointInRect(&left_top, &terrain[i].box)) && pastplayer.box.y >= terrain[i].box.y + terrain[i].box.h)
		{
			player.box.y = terrain[i].box.y + terrain[i].box.h;
			player.vely = 0;
		}
		//Keeps the player to the left of a rectangle
		else if (SDL_PointInRect(&right_bottom, &terrain[i].box) || SDL_PointInRect(&right_top, &terrain[i].box))
		{
			player.box.x = terrain[i].box.x - player.box.w;
			jump = true;
			wallcling = true;
			stomp = false;
		}
		//Keeps the player to the right of a rectangle
		else if (SDL_PointInRect(&left_bottom, &terrain[i].box) || SDL_PointInRect(&left_top, &terrain[i].box))
		{
			player.box.x = terrain[i].box.x + terrain[i].box.w;
			jump = true;
			wallcling = true;
			stomp = false;
		}
	}
}

void Update(float dt)
{
	pastplayer = player;
	if (!gen) 
	{
		TerGen();
	}
	PosUp(dt);
	ColUp(dt);
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		ExitGame();
	}
	if(bulletCooldown < 0) bulletCooldown = 0;
	else bulletCooldown -= 0.1;
}

void RenderFrame(float interpolation)
{
	
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 65, 105, 225, 255);
	SDL_RenderClear(gRenderer);

	SDL_SetRenderDrawColor(gRenderer, 160, 0, 160, 255);
	SDL_RenderFillRect(gRenderer, &player.box);
	SDL_SetRenderDrawColor(gRenderer, 120, 120, 120, 255);
	for (int i = 0; i < ammount; i++)
	{
		SDL_RenderFillRect(gRenderer, &terrain[i].box);
	}
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderCopyEx(gRenderer, TEXgun, NULL, &gun.box, result, &rotationCenter, SDL_FLIP_NONE);
	for (int i = 0; i <= 50; i++) {
		if(bullet[i].state == 0) break;
		else {
			SDL_RenderFillRect(gRenderer, &bullet[i].box);
		}
	}
}
