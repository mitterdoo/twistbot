#include <algorithm>
#include "MoveScore.h"

MoveScore::MoveScore()
{
	score = 0;
	levelScore = 0;
	levelScoreAccumulator = 0;
	cascades = 0;
	coalCount = 0;
}

void MoveScore::Reset()
{
	score = 0;
	levelScore = 0;
	levelScoreAccumulator = 0;
	cascades = 0;
	coalCount = 0;
}
void MoveScore::ScoreMatch(int matchLength)
{
	int result = 0;
	switch (matchLength)
	{
	case 3:
		result = 25;
		break;
	case 4:
		result = 50;
		break;
	default:
		result = 100;
	}
	AddScore(result, "Match");
}

void MoveScore::ScoreCascade()
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
	AddScore(result, "Cascade");
}

void MoveScore::ScoreCoal()
{
	int result = std::min(700, 350 + coalCount * 100) * multiplier;
	AddScoreNoMultiplier(result, "Coal geode");
	coalCount++;
}

void MoveScore::AddScore(int score, const char* msg)
{
	int result = score * multiplier;
	this->score += result;
	AddLevelScore(score, result);
}

void MoveScore::AddScore(SCORES score, const char* msg)
{
	int result = (int)score * multiplier;
	this->score += result;
	AddLevelScore((int)score, result);
}

void MoveScore::AddScoreNoMultiplier(int score, const char* msg)
{
	this->score += score;
	AddLevelScore(score, score);
}

void MoveScore::AddScoreNoMultiplier(SCORES score, const char* msg)
{
	this->score += (int)score;
	AddLevelScore((int)score, (int)score);
}

void MoveScore::AddLevelScore(int baseScore, int score)
{
	// TODO: there's extra bits of math in the original game at this point, but it seems to be redundant.. hoping it is

	int remaining = score;
	do
	{
		int step = 100;
		if (remaining < step)
			step = remaining;
		
		double result1 = pow((double)(this->levelScoreAccumulator + step), 0.75);
		double result2 = pow((double)(this->levelScoreAccumulator), 0.75);

		int scaled_score = (result1 - result2) * 3;
		levelScore += scaled_score;
		remaining -= step;

		this->levelScoreAccumulator += baseScore;

	} while (remaining > 0);
}
