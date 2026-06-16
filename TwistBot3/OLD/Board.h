#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include "Gem.h"
#include "Score.h"
#include "HandyTypes.hpp"

typedef std::list<Gem*> Match;

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
	int GetMatches(std::vector<Match>* matchesOut, std::vector<Match>* crossMatchesOut);

	int GetGemMatchCountFromMove(Vector2 pos);

	std::string GetString();

private:
	typedef struct Intersection
	{
		Match match1;
		Match match2;
		Gem* gem;
	} Intersection;
	const int HOP_LOOKUP[17] = { 0, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 8, -8 };

	char gemUpgrades[8][8];


	void DestroyGem(Vector2 pos);
	int DestroyRow(int row);
	int DestroyCol(int col);
	int DestroyRadius(Vector2 pos, int radius);
	Gem* HopGems(std::vector<Gem*> run);
	void Collapse();
	int FindMatchesInRun(Gem* run[8], std::vector<Match>* matchesOut);
	

};

