#include <algorithm>
#include "Score.h"

Score::Score()
{
	score = 0;
	cascades = 0;
	coalCount = 0;
}

void Score::Reset()
{
	score = 0;
	cascades = 0;
	coalCount = 0;
}
void Score::ScoreMatch(int matchLength)
{
	int result = 0;
	switch (matchLength)
	{
	case 3:
		//result = 50;
		result = 5;
		break;
	case 4:
		//result = 100;
		result = 10;
		break;
	default:
		//result = 200;
		result = 20;
	}
	score += result * multiplier;
}

void Score::ScoreCascade()
{
	int result = 0;
	switch (++cascades)
	{
	case 1:
		break;

	case 2:
		result = 50;
		break;

	case 3:
		result = 100;
		break;

	case 4:
		result = 150;
		break;

	case 5:
		result = 200;
		break;

	case 6:
		result = 300;
		break;
	
	default:
		result = 400;
	}
	score += result * multiplier;
}

void Score::ScoreCoal()
{
	int result = std::min(700, 350 + coalCount * 100) * multiplier;
	score += result;
	coalCount++;
}

void Score::AddScore(int score, const char* msg)
{
	this->score += score * multiplier;
}

void Score::AddScore(SCORES score, const char* msg)
{
	this->score += (int)score * multiplier;
}

void Score::AddScoreNoMultiplier(SCORES score)
{
	this->score += (int)score * multiplier;
}
