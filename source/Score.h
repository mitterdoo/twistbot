#pragma once

constexpr int SCORE_DESTROY_GEM = 1;
constexpr int SCORE_DESTROY_FLAME = 20;
constexpr int SCORE_DESTROY_LIGHTNING = 50;
constexpr int SCORE_DESTROY_NOVA = 150;
constexpr int SCORE_MEGAFRUIT = 50000;
constexpr int SCORE_DESTROY_FRUIT_BASE = 60;
constexpr double SCORE_DESTROY_FRUIT_PERGEM_MULT = 3.5;
constexpr int  SCORE_DESTROY_LIGHTNING_PERGEM = 2.0;
constexpr int SCORE_CREATE_FLAME = 5;
constexpr int SCORE_CREATE_LIGHTNING = 12;
constexpr int SCORE_CREATE_NOVA = 38;
constexpr int SCORE_CREATE_FRUIT = 42;
constexpr int SCORE_BONUS_PERLEVEL = 3000;

constexpr int SCORE_DESTROY_BOMB = 500000;
constexpr int SCORE_DESTROY_LOCK = 100;
constexpr int SCORE_COMBO_GAIN = 2000;
constexpr int SCORE_COMBO_MULTIPLIER = 100;

constexpr int SCORE_NULL = 0;

constexpr auto LOWEST_SCORE = -2000000000;
//constexpr auto LOWEST_SCORE = 0;
typedef enum SCORES
{
	SPECIAL_INTERSECTION = 50,
	SPECIAL_KILL_FLAME = 100,
	SPECIAL_KILL_LIGHTNING = 300,
	SPECIAL_KILL_LIGHTNING_PERGEM = 20,
	SPECIAL_KILL_FRUIT = 1000,
	SPECIAL_KILL_FRUIT_PERGEM_NORMAL = 20,
	SPECIAL_KILL_FRUIT_PERGEM_FLAME = 200,
	SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING = 300,
	SPECIAL_KILL_FRUIT_PERGEM_FRUIT = 2000,
	SPECIAL_KILL_BOMB = 100,
	SPECIAL_KILL_DOOM = 1000,
	SPECIAL_MEGAFRUIT = 71500,
	SPECIAL_CREATE_FLAME = 1000,
	SPECIAL_CREATE_LIGHTNING = 2000,
	SPECIAL_CREATE_NOVA = 5000,
	SPECIAL_CREATE_FRUIT = 7500,
	//SPECIAL_COMBO_BREAK_PER = -100,
	//SPECIAL_COMBO_BREAK = -2000,
	SPECIAL_COMBO_MAINTAIN = 500,
	SPECIAL_COMBO_LEVELUP = 800,

	LOCKING_DESTROY = 600,
	LOCK_DESTROY = 500,
	BOMB_DESTROY = 3000,
	DOOM_DESTROY = 5000,

	COUNTER_ZERO = -10000
} SCORES;

class Score
{
public:
	Score();

	int score;
	int cascades;
	int coalCount;
	int multiplier = 1;
	double multiplier2 = 1.0;

	void Reset();
	void ScoreMatch(int matchLength);
	void ScoreCascade();
	void ScoreCoal();
	void AddScore(int score, const char* msg = "");
	void AddScore(SCORES score, const char* msg = "");
	void AddScoreNoMultiplier(SCORES score);

};

