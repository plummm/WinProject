#include "stdafx.h"
#include "Aircraft.h"

#pragma comment(lib, "Shlwapi.lib")

Aircraft::Aircraft(int flag):
eTexture()
,eSprite()
{
	char path[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, path);

	switch (flag)
	{
	case Enemy:
		type = flag;
		HealthPoint = EnemyHP;
		Armor = EnemyArmor;
		Score = EnemyScore;
		damage = EnemyDamage;
		range = EnemyRange;
		father = Enemy;
		strcat_s(path, MAX_PATH, "\\resources\\image\\enemy.png");
		if (PathFileExistsA(path))
		{
			eTexture.loadFromFile(path);
		}
		
		eSprite.setTexture(eTexture);
		break;
	case Player:
		type = flag;
		eVelocity = PlayerSpeed;
		HealthPoint = PlayerHP;
		Armor = PlayerArmor;
		Score = PlayerScore;
		damage = PlayerDamage;
		range = PlayerRange;
		father = Player;
		underAttack = Safe;
		eTexture.loadFromFile("resources/image/player.png");
		eSprite.setTexture(eTexture);
		break;
	case Bullet:
		type = flag;
		eVelocity = BulletSpeed;
		HealthPoint = BulletHP;
		Armor = BulletArmor;
		Score = BulletScore;
		damage = BulletDamage;
		range = BulletRange;
		father = Player;
		ammo = MAXNUM;
		eTexture.loadFromFile("resources/image/bullet.png");
		eSprite.setTexture(eTexture);
		break;
	case JbBullet:
		type = flag;
		eVelocity = JbBulletSpeed;
		HealthPoint = JbBulletHP;
		Armor = JbBulletArmor;
		Score = JbBulletScore;
		damage = JbBulletDamage;
		range = JbBulletRange;
		father = Player;
		ammo = InitialJbBullet;
		eTexture.loadFromFile("resources/image/jbbullet.png");
		eSprite.setTexture(eTexture);
		break;
	case Boss:
		type = flag;
		HealthPoint = BossHP;
		Armor = BossArmor;
		Score = BossScore;
		damage = BossDamage;
		range = BossRange;
		father = Boss;
		eTexture.loadFromFile("resources/image/boss.png");
		eSprite.setTexture(eTexture);
		break;
	case Bomb:
		type = flag;
		eVelocity = BombSpeed;
		HealthPoint = BombHP;
		Armor = BombArmor;
		Score = BombScore;
		damage = BombDamage;
		range = BombRange;
		father = Player;
		ammo = InitialBomb;
		eTexture.loadFromFile("resources/image/bombtarget.png");
		eSprite.setTexture(eTexture);
		break;
	case ExplodeWave:
		type = flag;
		eVelocity = ExplodeWaveSpeed;
		HealthPoint = ExplodeWaveHP;
		Armor = ExplodeWaveArmor;
		Score = ExplodeWaveScore;
		damage = ExplodeWaveDamage;
		range = ExplodeWaveRange;
		father = Player;
		eTexture.loadFromFile("resources/image/explosivewave.png");
		eSprite.setTexture(eTexture);
		break;
	case ParachuteBomb:
		type = flag;
		eVelocity = ParachuteBombSpeed;
		HealthPoint = ParachuteBombHP;
		Armor = ParachuteBombArmor;
		Score = ParachuteBombScore;
		damage = ParachuteBombDamage;
		range = ParachuteBombRange;
		father = Parachute;
		eTexture.loadFromFile("resources/image/parachutebomb.png");
		eSprite.setTexture(eTexture);
		break;
	case ParachuteBullet:
		type = flag;
		eVelocity = ParachuteBulletSpeed;
		HealthPoint = ParachuteBulletHP;
		Armor = ParachuteBulletArmor;
		Score = ParachuteBulletScore;
		damage = ParachuteBulletDamage;
		range = ParachuteBulletRange;
		father = Parachute;
		eTexture.loadFromFile("resources/image/parachutejbbullet.png");
		eSprite.setTexture(eTexture);
		break;
	case Laser:
		type = flag;
		eVelocity = LaserSpeed;
		HealthPoint = LaserHP;
		Armor = LaserArmor;
		Score = LaserScore;
		damage = LaserDamage;
		range = LaserRange;
		father = Enemy;
		eTexture.loadFromFile("resources/image/laser.png");
		eSprite.setTexture(eTexture);
		break;
	case ParachuteCure:
		type = flag;
		eVelocity = ParachuteCureSpeed;
		HealthPoint = ParachuteCureHP;
		Armor = ParachuteCureArmor;
		Score = ParachuteCureScore;
		damage = ParachuteCureDamage;
		range = ParachuteCureRange;
		father = Parachute;
		eTexture.loadFromFile("resources/image/cure.png");
		eSprite.setTexture(eTexture);
		break;
	case CaiDan:
		type = flag;
		eVelocity = CaiDanSpeed;
		HealthPoint = CaiDanHP;
		Armor = CaiDanArmor;
		Score = CaiDanScore;
		damage = CaiDanDamage;
		range = CaiDanRange;
		father = Player;
		eTexture.loadFromFile("resources/image/caidan.png");
		eSprite.setTexture(eTexture);
		break;
	default:
		break;
	}
	

}


Aircraft::~Aircraft()
{
}
