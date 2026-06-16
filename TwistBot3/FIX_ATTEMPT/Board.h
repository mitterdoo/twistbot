#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Gem.h"
#include "Score.h"
#include "HandyTypes.hpp"

typedef struct Match
{
	int size;
	Gem* gems[8];

	inline Gem* Get(int index)
	{
		return gems[index];
	}

	inline void Push(Gem* gem)
	{
		if (size >= 8)
			throw new std::runtime_error("Array at max size!");
		gems[size++] = gem;
	}

	void Erase(Gem* gem)
	{
		char found = 0;
		for (int i = 0; i < 8; i++)
		{
			if (gems[i] == gem)
				found = 1;
			if (found && i < 7)
				gems[i] = gems[i + 1];
		}
		size -= found;
	}
} Match;

typedef struct Bonus {
	GemColor colors[4];
	int index;
	char fruitBonus;
} Bonus;

class Board
{
public:
	Board();
	~Board();

	Gem gems[8][8];

	Bonus bonus;
	Score score;

	void Rotate(Vector2 pos);
	void AntiRotate(Vector2 pos);

	int RunMatch();
	Board* Copy();

	/// <summary>
	/// Gets all matches on the board
	/// </summary>
	/// <param name="matches">Horizontal and vertical matches</param>
	/// <param name="crossMatches">Intersecting matches</param>
	/// <returns>Score</returns>
	int GetMatches(std::vector<Match*>* matchesOut, std::vector<Match*>* crossMatchesOut);

	int GetGemMatchCountFromMove(Vector2 pos);

	std::string GetString();

private:
	typedef struct Intersection
	{
		Match* match1;
		Match* match2;
		Gem* gem;
	} Intersection;
	const int HOP_LOOKUP[17] = { 0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 8, -8 };

	Match _matches[64];
	int _matches_s;
	char gemUpgrades[8][8];


	void DestroyGem(Vector2 pos);
	int DestroyRow(int row);
	int DestroyCol(int col);
	int DestroyRadius(Vector2 pos, int radius);
	Gem* HopGems(Match* run);
	void Collapse();
	int FindMatchesInRun(Gem* run[8]);
	

};

