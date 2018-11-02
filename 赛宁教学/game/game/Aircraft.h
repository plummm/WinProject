#pragma once
#include "stdafx.h"


class Aircraft
{
public:
	void crash();
	Aircraft(int flag);
	~Aircraft();

public:
	sf::Sprite eSprite;
	sf::Texture eTexture;
	sf::IntRect eRect;

	

public:
	float eVelocity;
	int HealthPoint, Score, Armor,width,height;
	int damage, range, ammo;
	int flag, type;
	int father;
	bool isFriend, underAttack;

};

