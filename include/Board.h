#pragma once
#include <cstdint>
#include <math.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <stdlib.h>
#include "Gem.h"
#include "Score.h"
#include "HandyTypes.hpp"
using namespace std;

//typedef std::list<Gem*> Match;
typedef struct Match
{
	bool isIntersection = false;
//	std::list<Gem*> match;
	Gem* match[16];
	int match_size = 0;
} Match;

typedef struct Bonus {
	GemColor colors[4];
	int index;
	char fruitBonus;
} Bonus;

const int COMBO_LOOKUP[10] = {4, 8, 12, 16, 16, 16, 16, 16, 16, 32};
typedef struct ComboMeter {
	int multiplier = 1;
	int count = 0;
} ComboMeter;

typedef enum ComboResult
{
	NONE,
	LEVELUP,
	EARN_FRUIT
} ComboResult;

constexpr uint32_t MATCHRESULT_NONE =		0x00000000;
constexpr uint32_t MATCHRESULT_MATCHED =	0x00000001;
constexpr uint32_t MATCHRESULT_DEFUSED =	0x00000002;
constexpr uint32_t MATCHRESULT_DEFUSED_DANGER = 0x00000004;
constexpr uint32_t MATCHRESULT_KILLDOOM =	0x00000008;
constexpr uint32_t MATCHRESULT_BOMBZERO =	0x00000010;

class Board
{
public:
	Board();
	~Board();

	Gem gems[8][8];
	Bonus bonus;
	Score score;
	ComboMeter comboMeter;
	uint32_t matchResultFlags = MATCHRESULT_NONE;
	// keeps track of all null moves made
	char movesMade[7][7];
	int lowestBomb = -1;


	bool gameOver = false;
	
	// returns success
	int Rotate(Vec2 pos);
	void AntiRotate(Vec2 pos);

	void SetComboMeter(int combo);

	// returns whether levelled up
	ComboResult ComboAdd();

	// returns how many ticks we fell
	int ComboBreak();

	int RunMatch(bool autoFill = false);
	Board* Copy();

	/// <summary>
	/// Gets all matches on the board
	/// </summary>
	/// <param name="matches">Horizontal and vertical matches</param>
	/// <param name="crossMatches">Intersecting matches</param>
	/// <returns>Score</returns>
	//int GetMatches(std::vector<Match>* matchesOut, std::vector<Match>* crossMatchesOut);

	/// <summary>
	/// Get matches in the board's current state
	/// </summary>
	/// <param name="matchesOut">Reference to vector of matches</param>
	/// <returns>Number of matches + cross matches</returns>
	int GetMatches(std::vector<Match>* matchesOut);

	bool ContainsMatch();
	bool ContainsPossibleMatch();

	/// <summary>
	/// Uses optimized match algorithm to check if the Gem is in a match
	/// </summary>
	/// <param name="gem"></param>
	/// <returns></returns>
	bool CheckIfGemInMatch(Gem* gem);

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

	int matchlessMoves = 0;

	inline void ClearMovesMade()
	{
		memset((void*)movesMade, 0, sizeof(char) * 7 * 7);
	}

	void FillRandomly();
	void DestroyGem(Vec2 pos);
	int DestroyRow(int row);
	int DestroyCol(int col);
	int DestroyRadius(Vec2 pos, int radius);
	Gem* HopGems(Match* run);
	void Collapse();
	

};

