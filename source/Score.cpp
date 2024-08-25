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
	score += result * multiplier * multiplier2;
	//printf("$$ %i\tScore::ScoreMatch(%i)\n", result * multiplier, matchLength);
}

void Score::ScoreCascade()
{
	int result = 0;
	switch (++cascades)
	{
	case 1:
		break;

	case 2:
		//result = 50;
		result = 5;
		break;

	case 3:
		//result = 100;
		result = 10;
		break;

	case 4:
		//result = 150;
		result = 15;
		break;

	case 5:
		//result = 200;
		result = 20;
		break;

	case 6:
		//result = 300;
		result = 30;
		break;
	
	default:
		//result = 400;
		result = 40;
	}
	score += result * multiplier * multiplier2;
	//printf("$$ %i\tScore::ScoreCascade() (%ith)\n", result * multiplier, cascades);
}

void Score::ScoreCoal()
{
	//int result = std::min(700, 350 + coalCount * 100) * multiplier;
	int result = std::min(70, 35 + coalCount * 10) * multiplier * multiplier2;
	score += result;

	//printf("$$ %i\tScore::ScoreCoal() (%ith)\n", result, coalCount);
	coalCount++;
}

void Score::AddScore(int score, const char* msg)
{
	this->score += score * multiplier * multiplier2;
	//printf("$$ %i\tScore::AddScore(\"%s\")\n", score * multiplier, msg);
}

void Score::AddScore(SCORES score, const char* msg)
{
	this->score += (int)score * multiplier * multiplier2;
	//printf("$$ %i\tScore::AddScore(\"%s\")\n", (int)(score * multiplier), msg);
}

void Score::AddScoreNoMultiplier(SCORES score)
{
	this->score += (int)score * multiplier;
}
