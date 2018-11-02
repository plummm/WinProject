#pragma once
#include "Aircraft.h"


class Game
{
public:
	Game();
	void run();
	void menu();
	~Game();

private:
	void checkCollision(PFITERATOR object1, PFITERATOR object2);
	bool checkOverstep(PFITERATOR object);
	void changeBullet(int flag);
	void enemyup();
	void enemyshoot();
	void entityDestroy();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void layout();
	void listup();
	void parachuteup();
	void processDestroy(PFITERATOR object);
	void processEvents();
	void render();
	void showScore(float x,float y);
	void update(sf::Time deltaTime);
	void release();
	char* Game::decode(int* raw, int length);


private:
	bool mIsMovingUp, mIsMovingDown, mIsMovingLeft, mIsMovingRight, mFire, mBullet, mJbBullet, mBomb, mUp, mDown, mLeft, mRight, mReturn, mEsc, bombstop, upstop, downstop, if_menu, if_pause, if_caiDan;
	int bombType, bossPositionX[100], limitEnemy, limitBoss;
	char seed[33] = "b4b3877bfda9898f77facace41f0d003";
	bool is_release = false;
	int enemyNum, bossNum, sumScore, gameOverFlag, count, parachuteBombScore, parachuteBulletScore, parachuteCureScore, lastHP;

private:
	void fire(Aircraft mPlayer);
	BUTTON buttonList[10],currentButton;
	std::list<Aircraft> events,backup;
	std::list<PFITERATOR> destroy;
	std::wstring scoreString;
	Aircraft mPlayer, *bullet, enemy, commonBullet, jbBullet, boss, bomb, explodeWave, parachuteBomb,parachuteBullet, parachuteCure, laser, caiDan;

private:
	sf::Time globalTime, fireTime;
	sf::Sprite backGround1, backGround2, gameOver, sBullet, sJbBullet, sFrame, sBomb, sBombTarget, sHealthPoint, sMenu, sNewGameButton, sExitButton, sArrow, sPause, sBackToMenu, sHelpButton, sHelp;
	sf::Sprite sAttackSideLeft[13];
	sf::Texture tBackGround, tGameOver, tBullet, tJbBullet, tFrame, tBomb, tBombTarget, tHealthPoint, tMenu, tNewGameButton, tExitButton, tArrow, tPause, tBackToMenu, tHelpButton, tHelp;
	sf::Texture tAttackSideLeft[13];
	sf::Clock fireClock;
	sf::RenderWindow mWindow;
	sf::Font font;
	sf::Text text, jbBulletAmmo, bombAmmo;
	sf::Music music, bulletSound, playerAttackSound, enemyDestroySound, gameOverSound, bossSound, bossDestroy, bombExplode, getBomb, getJbBullet, laserSound, cure, menuSound, buttonSound, caidanSound;
};

