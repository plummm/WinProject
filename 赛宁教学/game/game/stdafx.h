// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once



#include "targetver.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <memory>
#include <list>
#include <algorithm>
#include <Windows.h>
#include <ctime>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string>
#include <math.h>
#include <shlwapi.h>


typedef 
struct button
{
	int positionX, positionY;
	int currentNum;
}BUTTON;

//typedef std::list<>


#define Enemy 0
#define Player 1
#define Bullet 2
#define JbBullet 3
#define Bomb 4
#define Boss 5
#define ExplodeWave 6
#define ParachuteBomb 7
#define ParachuteBullet 8
#define Laser 9
#define ParachuteCure 10
#define Parachute 11
#define CaiDan 12
#define ExtraRate 1.5
#define Alive 1
#define Destroy1 99999985
#define Destroy2 99999990
#define Destroy3 99999995
#define Destroy4 99999999
#define Gameover1 100000135
#define Gameover2 100000205
#define Gameover3 100000275
#define Gameover4 100000365
#define ArmorRate 4

// TODO:  在此处引用程序需要的其他头文件
#define PlayerSpeed 300.f
#define PlayerHP 1000
#define PlayerScore 0
#define PlayerArmor 5
#define PlayerDamage 300
#define PlayerRange 0



//Enemy
#define EnemySpeed 100.f
#define EnemyHP 300
#define EnemyScore 50
#define EnemyArmor 0
#define EnemyDamage 200;
#define EnemyRange 0

//bullet
#define BulletSpeed 0-400.f
#define BulletHP 1
#define BulletScore 0
#define BulletArmor 0
#define BulletDamage 50
#define BulletRange 0

//jbbullet
#define JbBulletSpeed 0-400.f
#define JbBulletHP 1
#define JbBulletScore 0
#define JbBulletArmor 0
#define JbBulletDamage 100
#define JbBulletRange 0

//bomb
#define BombSpeed 500.f
#define BombHP 99999999
#define BombScore 0
#define BombArmor 0
#define BombDamage 0
#define BombRange 0

//explosive wave
#define ExplodeWaveSpeed 700.f
#define ExplodeWaveHP 99999999
#define ExplodeWaveScore 0
#define ExplodeWaveArmor 0
#define ExplodeWaveDamage 7000
#define ExplodeWaveRange 0

//boss
#define BossSpeed 20.f
#define BossHP 5000
#define BossScore 1000
#define BossArmor 8
#define BossDamage 1000;
#define BossRange 0

//parachute bomb
#define ParachuteBombSpeed 100.f
#define ParachuteBombHP 1
#define ParachuteBombScore 0
#define ParachuteBombArmor 0
#define ParachuteBombDamage 0
#define ParachuteBombRange 0

//parachute bullet
#define ParachuteBulletSpeed 100.f
#define ParachuteBulletHP 1
#define ParachuteBulletScore 0
#define ParachuteBulletArmor 0
#define ParachuteBulletDamage 0
#define ParachuteBulletRange 0

//parachute bullet
#define ParachuteCureSpeed 100.f
#define ParachuteCureHP 1
#define ParachuteCureScore 0
#define ParachuteCureArmor 0
#define ParachuteCureDamage 0
#define ParachuteCureRange 0

//laser
#define LaserSpeed 800.f
#define LaserHP 1
#define LaserScore 0
#define LaserArmor 0
#define LaserDamage 22
#define LaserRange 0

//caidan
#define CaiDanSpeed 150.f
#define CaiDanHP 9999999
#define CaiDanScore 0
#define CaiDanArmor 0
#define CaiDanDamage 10000
#define CaiDanRange 0


//limit boundary
#define BoundaryHigh 800
#define BoundaryWidth 480
#define BulletPositionX 190.f
#define BulletPositionY 765.f
#define JbBulletPositionX 240.f
#define JbBulletPositionY 765.f
#define BombPositionX 290.f
#define BombPositionY 765.f

//time
#define Level1Time 490
#define Level2Time 440
#define Level3Time 390


//other
#define GetBombs 1
#define PFITERATOR std::_List_iterator<std::_List_val<std::_List_simple_types<Aircraft>>>
#define NoFather 9999
#define MAXNUM 999999999
#define InitialJbBullet 400
#define InitialBomb 3
#define BackGroundSpeed 50.f
#define M_PI 3.14159265358979323846
#define UnderAttack true
#define Safe false
#define offset 25
#define NewGameButtonX 95
#define NewGameButtonY 320
#define HelpButtonX 95
#define HelpButtonY 450
#define ExitButtonX 95
#define ExitButtonY 580
#define MaxNumOfenemy 1000
#define MaxNumOfBoss 14