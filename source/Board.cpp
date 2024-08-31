#include "Board.h"

Board::Board()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			gems[x][y].pos = { x, y };
		}
	}
	bonus = {
		{GemColor::RED, GemColor::RED, GemColor::RED, GemColor::RED},
		0, 0
	};
	memset((void*)gemUpgrades, 0, sizeof(char) * 64);
	ClearMovesMade();
}

Board* Board::Copy()
{
	Board* board = new Board();
	memcpy((void*)board, (void*)this, sizeof(Board));
	return board;
}

Board::~Board()
{
}

int Board::Rotate(Vec2 pos)
{
	int x = pos.x;
	int y = pos.y;
	Gem buffer[4] = {gems[x][y], gems[x + 1][y], gems[x + 1][y + 1], gems[x][y + 1]};

	if (buffer[0].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
		buffer[1].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
		buffer[2].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
		buffer[3].flags & (GemFlags::LOCKED | GemFlags::DOOM))
	{
		return 0;
	}

	buffer[0].rotated = 1;
	buffer[1].rotated = 1;
	buffer[2].rotated = 1;
	buffer[3].rotated = 1;
	gems[x][y] = buffer[3].Move(x, y);
	gems[x + 1][y] = buffer[0].Move(x+1, y);
	gems[x + 1][y + 1] = buffer[1].Move(x+1, y+1);
	gems[x][y + 1] = buffer[2].Move(x,y+1);

	movesMade[pos.x][pos.y]++;

	return 1;
}

void Board::AntiRotate(Vec2 pos)
{
	int x = pos.x;
	int y = pos.y;
	Gem buffer[4] = {gems[x][y], gems[x + 1][y], gems[x + 1][y + 1], gems[x][y + 1]};
	gems[x][y] = buffer[1].Move(x, y);
	gems[x + 1][y] = buffer[2].Move(x + 1, y);
	gems[x + 1][y + 1] = buffer[3].Move(x + 1, y + 1);
	gems[x][y + 1] = buffer[0].Move(x, y + 1);

	movesMade[pos.x][pos.y]--;
}

void Board::SetComboMeter(int combo)
{
	combo = std::min(152, combo);
	comboMeter.multiplier = 1;
	while (combo >= COMBO_LOOKUP[comboMeter.multiplier - 1])
	{
		combo -= COMBO_LOOKUP[comboMeter.multiplier++ - 1];
	}
	comboMeter.count = combo;
}

ComboResult Board::ComboAdd()
{
	score.AddScore(SCORES::SPECIAL_COMBO_MAINTAIN, "COMBO MAINTAIN");
	if (++comboMeter.count >= COMBO_LOOKUP[comboMeter.multiplier-1])
	{
		comboMeter.count = 1;
		score.AddScore(SCORES::SPECIAL_COMBO_LEVELUP, "COMBO LEVELUP");
		if (comboMeter.multiplier == 10)
		{
			score.AddScore(SCORES::SPECIAL_CREATE_FRUIT, "SPECIAL_CREATE_FRUIT");
			return ComboResult::EARN_FRUIT;
		}
		else
		{
			comboMeter.multiplier++;
			return ComboResult::LEVELUP;
		}
	}
	return ComboResult::NONE;
}

int Board::ComboBreak()
{
	if (comboMeter.count > 0)
	{
		int lost = comboMeter.count;
		comboMeter.count = 0;
		return lost;
	}
	else
	{
		comboMeter.multiplier = std::max(comboMeter.multiplier - 1, 1);
		return COMBO_LOOKUP[comboMeter.multiplier - 1];
	}
}


int Board::RunMatch(bool autoFill)
{
	score.Reset();
	this->lowestBomb = -1;
	matchResultFlags = MATCHRESULT_NONE;
	for (int i = 0; i < 32; i++)
	{
		memset((void*)gemUpgrades, 0, sizeof(char) * 64);
		std::vector<Match> matches;
		int matchCount = GetMatches(&matches);

		if (i == 0)
		{
			int lowestBomb = 999;
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					Gem* gem = &gems[x][y];
					bool isBomb = gem->Is(GemFlags::BOMB);
					bool isDoom = gem->Is(GemFlags::DOOM);
					bool isLocking = gem->locking;
					if (isBomb && !isDoom)
					{
						gem->count--;
					}
					else if (isBomb && isDoom && matchCount == 0)
					{
						gem->count--;
					}
					else if (isLocking)
					{
						gem->flags = (GemFlags)(gem->flags | GemFlags::LOCKED);
					}

					if ((isBomb) && gem->count < lowestBomb)
					{
						lowestBomb = gem->count;
					}
				}
			}
			this->lowestBomb = lowestBomb;
		}

		if (matchCount == 0)
		{
			if (i == 0)
			{
				ComboBreak();
				bonus.index = 0;
				score.AddScore(SCORE_NULL);
			}
			break;
		}

		if (i == 0)
		{
			matchResultFlags |= MATCHRESULT_MATCHED;
			ClearMovesMade();
			ComboAdd();
		}

		score.ScoreCascade();

		int matchColorCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		for (Match match : matches)
		{
			if (match.isIntersection)
			{
				Gem newGem = match.match[0]->Copy();
				newGem.flags = GemFlags::FLAME;

				matchColorCount[(int)match.color]++;
				score.ScoreMatch(3);
				score.ScoreMatch(3);
				for (int i = 0; i < match.match_size; i++)
				{
					Gem* gem = match.match[i];
					DestroyGem(gem->pos);
				}
				gems[newGem.pos.x][newGem.pos.y] = newGem;
				gemUpgrades[newGem.pos.x][newGem.pos.y] = 1;
			}
			else
			{
				score.ScoreMatch(match.match_size);
				matchColorCount[(int)match.color]++;

				int length = match.match_size;
				if (length >= 4)
				{
					Gem* picked = HopGems(&match);
					if (length == 4)
					{
						picked->flags = (GemFlags)(picked->flags | GemFlags::FLAME);
						score.AddScore(SCORES::SPECIAL_CREATE_FLAME, "SPECIAL_CREATE_FLAME");

					}
					else if (length == 5)
					{
						picked->flags = (GemFlags)(picked->flags | GemFlags::LIGHTNING);
						score.AddScore(SCORES::SPECIAL_CREATE_LIGHTNING, "SPECIAL_CREATE_LIGHTNING");
					}
					else
					{
						picked->flags = (GemFlags)((GemFlags)(picked->flags | GemFlags::LIGHTNING) | GemFlags::FLAME);
						score.AddScore(SCORES::SPECIAL_CREATE_NOVA, "SPECIAL_CREATE_NOVA");
					}
					gemUpgrades[picked->pos.x][picked->pos.y] = 1;

				}

				for (int i = 0; i < match.match_size; i++)
				{
					Gem* gem = match.match[i];
					DestroyGem(gem->pos);
				}
			}
		}

		// bonus
		if (bonus.index < 4)
		{
			// allow going from 3rd bonus to 1st if 1st was matched
			if (matchColorCount[(int)bonus.colors[bonus.index]] == 0)
			{
				bonus.index = 0;
				if (bonus.fruitBonus)
				{
					bonus.index = 5;
					bonus.fruitBonus = false;
					bonus.colors[0] = GemColor::EMPTY;
					bonus.colors[1] = GemColor::EMPTY;
					bonus.colors[2] = GemColor::EMPTY;
					bonus.colors[3] = GemColor::EMPTY;
				}
			}

			for (int i = bonus.index; i < 4; i++)
			{
				GemColor neededColor = bonus.colors[i];
				if (matchColorCount[(int)neededColor] > 0)
				{
					bonus.index++;
					matchColorCount[(int)neededColor]--;

					score.AddScore((i+1) * SCORE_BONUS_PERLEVEL * (bonus.fruitBonus ? 5 : 1));

					if (bonus.index == 4 && bonus.fruitBonus)
					{
						printf("REWARDING FRUIT BONUS GET\n");
						score.AddScore(SCORES::SPECIAL_MEGAFRUIT, "SPECIAL_MEGAFRUIT");
					}
				}
				else
				{
					break;
				}
			}
		}

		Collapse();

		if (autoFill)
		{
			FillRandomly();
		}

		if (i >= 30)
		{
			printf("\nbad\n");
			printf(GetString().c_str());
			printf("");
		}
	}

	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			Gem* gem = &gems[x][y];
			bool isBomb = gem->Is(GemFlags::BOMB);
			bool isDoom = gem->Is(GemFlags::DOOM);
			if ((isBomb || isDoom) && gem->count == 0)
			{
				gameOver = true;
				matchResultFlags |= MATCHRESULT_BOMBZERO;
			}
		}
	}
	if (matchlessMoves > 0 && (matchResultFlags & MATCHRESULT_DEFUSED) == 0)
	{
		// don't reward a run of matchless moves unless we've defused a bomb
		//score.score = 0;
	}
	if (matchResultFlags & MATCHRESULT_MATCHED)
		matchlessMoves = 0;
	else
		matchlessMoves++;

	int finalScore = score.score;

	return finalScore;
}


/*
* 
*	gemPassCount[8][8]
*	matches_h = GetHorizontalMatches() // increments gem's pass count by 1 for each matching gem
*	intersections = []
*	matches_v = GetVerticalMatches() // increments gem's pass count by 1, add to intersections if already 1
* 
*/

#define VEC2INT(x, y) x + y*8

int Board::GetMatches(std::vector<Match>* matchesOut)
{

	Match* matches = new Match[128];
	int matches_size = 0;
	Match** matchMap = new Match*[64];
	memset(matches, 0, sizeof(Match) * 128);
	memset(matchMap, 0, sizeof(Match*) * 64);

	// find horizontal matches
	for (int y = 0; y < 8; y++)
	{
		const int MINIMUM = 3;
		int matchCount = 0;

		Gem* currentMatch[8];
		int currentMatch_count = 0;

		for (int x = 0; x < 8; x++)
		{
			Gem* gem = &gems[x][y];
			GemColor color = gem->color;
			if (color != GemColor::EMPTY &&
				(currentMatch_count == 0 || currentMatch[0]->color == color && color != GemColor::COAL))
			{
				currentMatch[currentMatch_count++] = gem;
			}
			else
			{
				if (currentMatch_count >= MINIMUM)
				{
					Match* match = &matches[matches_size++];
					match->match_size = currentMatch_count;
					memcpy((void*)match->match, (void*)currentMatch, sizeof(Gem*) * currentMatch_count);
					match->color = match->match[0]->color; // hoping this doesn't cause errors
					for (int j = 0; j < currentMatch_count; j++)
					{
						matchMap[VEC2INT(currentMatch[j]->pos.x, currentMatch[j]->pos.y)] = match;
					}
					matchCount++;
				}

				currentMatch_count = 0;
				if (color != GemColor::EMPTY)
				{
					currentMatch[currentMatch_count++] = gem;
				}
			}
		}

		if (currentMatch_count >= MINIMUM)
		{
			Match* match = &matches[matches_size++];
			match->match_size = currentMatch_count;
			memcpy((void*)match->match, (void*)currentMatch, sizeof(Gem*) * currentMatch_count);
			
			match->color = match->match[0]->color; // hoping this doesn't cause errors
			for (int j = 0; j < currentMatch_count; j++)
			{
				matchMap[VEC2INT(currentMatch[j]->pos.x, currentMatch[j]->pos.y)] = match;
			}
			matchCount++;
		}
	}

	// find VERTICAL matches
	for (int x = 0; x < 8; x++)
	{
		const int MINIMUM = 3;
		int matchCount = 0;

		Gem* currentMatch[8];
		int currentMatch_count = 0;
		Match* currentMatch_intersectionMatch = nullptr;
		Vec2 currentMatch_intersectionPoint = {-1, -1};

		for (int y = 0; y < 8; y++)
		{
			Gem* gem = &gems[x][y];
			GemColor color = gem->color;
			if (color != GemColor::EMPTY &&
				(currentMatch_count == 0 || currentMatch[0]->color == color && color != GemColor::COAL))
			{
				currentMatch[currentMatch_count++] = gem;
				if (matchMap[VEC2INT(x, y)] != nullptr)
				{
					currentMatch_intersectionMatch = matchMap[VEC2INT(x, y)];
					currentMatch_intersectionPoint = {x, y};
				}
			}
			else
			{
				if (currentMatch_count >= MINIMUM)
				{
					if (currentMatch_intersectionMatch != nullptr && currentMatch_intersectionMatch->match_size == currentMatch_count)
					{
						Match* match = currentMatch_intersectionMatch;
						match->isIntersection = true;

						for (int i = 0; i < match->match_size; i++)
						{
							Gem* foundGem = match->match[i];
							if (foundGem->pos == currentMatch_intersectionPoint)
							{
								std::swap(match->match[0], match->match[i]);
								break;
							}
						}

						for (int j = 0; j < currentMatch_count; j++)
						{
							if (currentMatch[j]->pos == currentMatch_intersectionPoint) continue;
							match->match[match->match_size++] = currentMatch[j];
						}
						match->color = match->match[0]->color; // hoping this doesn't cause errors
					}
					else
					{
						Match* match = &matches[matches_size++];
						match->match_size = currentMatch_count;
						memcpy((void*)match->match, (void*)currentMatch, sizeof(Gem*)* currentMatch_count);
						match->color = match->match[0]->color; // hoping this doesn't cause errors
						for (int j = 0; j < currentMatch_count; j++)
						{
							matchMap[VEC2INT(currentMatch[j]->pos.x, currentMatch[j]->pos.y)] = match;
							currentMatch_intersectionPoint = {x, y};
						}
						matchCount++;
					}
				}

				currentMatch_count = 0;
				currentMatch_intersectionMatch = nullptr;
				currentMatch_intersectionPoint = {-1, -1};
				if (color != GemColor::EMPTY)
				{
					currentMatch[currentMatch_count++] = gem;
					if (matchMap[VEC2INT(x, y)] != nullptr)
					{
						currentMatch_intersectionMatch = matchMap[VEC2INT(x, y)];
					}
				}
			}
		}

		if (currentMatch_count >= MINIMUM)
		{
			if (currentMatch_intersectionMatch != nullptr && currentMatch_intersectionMatch->match_size == currentMatch_count)
			{
				Match* match = currentMatch_intersectionMatch;
				match->isIntersection = true;

				for (int i = 0; i < match->match_size; i++)
				{
					Gem* foundGem = match->match[i];
					if (foundGem->pos == currentMatch_intersectionPoint)
					{
						std::swap(match->match[0], match->match[i]);
						break;
					}
				}

				for (int j = 0; j < currentMatch_count; j++)
				{
					if (currentMatch[j]->pos == currentMatch_intersectionPoint) continue;
					match->match[match->match_size++] = currentMatch[j];
				}
				match->color = match->match[0]->color; // hoping this doesn't cause errors
			}
			else
			{
				Match* match = &matches[matches_size++];
				match->match_size = currentMatch_count;
				memcpy((void*)match->match, (void*)currentMatch, sizeof(Gem*) * currentMatch_count);
				match->color = match->match[0]->color; // hoping this doesn't cause errors
				for (int j = 0; j < currentMatch_count; j++)
				{
					matchMap[VEC2INT(currentMatch[j]->pos.x, currentMatch[j]->pos.y)] = match;
				}
				matchCount++;
			}
		}
	}

	for (int i = 0; i < matches_size; i++)
	{
		matchesOut->push_back(matches[i]);
	}


	delete[] matchMap;
	delete[] matches;

	return matches_size;

}

bool Board::CheckIfGemInMatch(Gem* gem)
{
	GemColor col = gem->color;
	if (col == GemColor::COAL || col == GemColor::EMPTY) return false;

	Vec2 pos = gem->pos;
	// check vert

	int count = 1;
	bool stop_up = false;
	bool stop_down = false;
	bool stop_left = false;
	bool stop_right = false;
	for (int i = 1; i <= 2; i++)
	{
		int y_up = pos.y - i;
		int y_down = pos.y + i;

		if (!stop_up && y_up >= 0 && gems[pos.x][y_up].color == col)
			count++;
		else
			stop_up = true;

		if (!stop_down && y_down < 8 && gems[pos.x][y_down].color == col)
			count++;
		else
			stop_down = true;
	}
	if (count >= 3)
	{
		return true;
	}

	count = 1;
	for (int i = 1; i <= 2; i++)
	{
		int x_left = pos.x - i;
		int x_right = pos.x + i;

		if (!stop_left && x_left >= 0 && gems[x_left][pos.y].color == col)
			count++;
		else
			stop_left = true;

		if (!stop_right && x_right < 8 && gems[x_right][pos.y].color == col)
			count++;
		else
			stop_right = true;
	}
	if (count >= 3)
		return true;
	return false;

}

bool Board::ContainsMatch()
{
	const int MINIMUM = 3;

	Gem* currentMatch[8];
	int currentMatch_count = 0;

	for (int y = 0; y < 8; y++)
	{
		currentMatch_count = 0;
		for (int x = 0; x < 8; x++)
		{
			Gem* gem = &gems[x][y];
			if (gem->color != GemColor::EMPTY &&
				(currentMatch_count == 0 || currentMatch[0]->color == gem->color && gem->color != GemColor::COAL))
			{
				currentMatch[currentMatch_count++] = gem;
			}
			else
			{
				if (currentMatch_count >= MINIMUM)
				{
					return true;
				}

				currentMatch_count = 0;
				if (gem->color != GemColor::EMPTY)
				{
					currentMatch[currentMatch_count++] = gem;
				}
			}

		}

		if (currentMatch_count >= MINIMUM)
		{
			return true;
		}
	}

	for (int x = 0; x < 8; x++)
	{
		currentMatch_count = 0;
		for (int y = 0; y < 8; y++)
		{
			Gem* gem = &gems[x][y];
			if (gem->color != GemColor::EMPTY &&
				(currentMatch_count == 0 || currentMatch[0]->color == gem->color && gem->color != GemColor::COAL))
			{
				currentMatch[currentMatch_count++] = gem;
			}
			else
			{
				if (currentMatch_count >= MINIMUM)
				{
					return true;
				}

				currentMatch_count = 0;
				if (gem->color != GemColor::EMPTY)
				{
					currentMatch[currentMatch_count++] = gem;
				}
			}

		}

		if (currentMatch_count >= MINIMUM)
		{
			return true;
		}
	}
	return false;
}

bool Board::ContainsPossibleMatch()
{
	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			Rotate({x, y});
			if (ContainsMatch())
			{
				AntiRotate({x, y});
				return true;
			}
			AntiRotate({x, y});
		}
	}
	return false;
}

std::string Board::GetString()
{
	std::string str;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			str.append(gems[x][y].StringColor());
		}
		str.append("\n");
	}
	str.append("----------------\n");
	str.append("BONUS: ");
	for (int i = 0; i < 4; i++)
	{
		if (i == bonus.index)
			str.append(">");

		str.append(Gem::GEM_COLORS[(int)bonus.colors[i]]);
	}
	if (bonus.fruitBonus)
		str.append("\tFRUIT BONUS");

	return str;
	
}

void Board::DestroyGem(Vec2 pos)
{
	// player just upgraded this gem. do not destroy
	if (gemUpgrades[pos.x][pos.y]) return;
	if (gems[pos.x][pos.y].color == GemColor::EMPTY) return;

	Gem gem = gems[pos.x][pos.y]; // create a copy
	gems[pos.x][pos.y] = Gem({pos.x, pos.y}); // put empty gem here

	if (gem.Is(GemFlags::COAL))
	{
		score.ScoreCoal();
	}
	else if (gem.Is(GemFlags::FLAME) && !gem.Is(GemFlags::LIGHTNING))
	{
		score.AddScore(SCORES::SPECIAL_KILL_FLAME, "SPECIAL_KILL_FLAME");
		DestroyRadius(gem.pos, 3);
	}
	else if (gem.Is(GemFlags::LIGHTNING) && !gem.Is(GemFlags::FLAME))
	{
		int count = 1;
		score.AddScore(SCORES::SPECIAL_KILL_LIGHTNING, "SPECIAL_KILL_LIGHTNING");
		count += DestroyRow(gem.pos.y);
		count += DestroyCol(gem.pos.x);
		score.AddScore((int)SCORES::SPECIAL_KILL_LIGHTNING_PERGEM * count, "SPECIAL_KILL_LIGHTNING_PERGEM");
	}
	else if (gem.Is(GemFlags::LIGHTNING) && gem.Is(GemFlags::FLAME))
	{
		int count = 1;
		score.AddScore(SCORES::SPECIAL_KILL_LIGHTNING, "SPECIAL_KILL_LIGHTNING (SUPERNOVA)");
		count += DestroyRow(gem.pos.y - 1);
		count += DestroyRow(gem.pos.y);
		count += DestroyRow(gem.pos.y + 1);

		count += DestroyCol(gem.pos.x - 1);
		count += DestroyCol(gem.pos.x);
		count += DestroyCol(gem.pos.x + 1);
		score.AddScore((int)SCORES::SPECIAL_KILL_LIGHTNING_PERGEM * count, "SPECIAL_KILL_LIGHTNING_PERGEM");
	}
	else if (gem.Is(GemFlags::FRUIT))
	{
		score.AddScore(SCORES::SPECIAL_KILL_FRUIT, "SPECIAL_KILL_FRUIT");
		int cRegular = 0;
		int cFlame = 0;
		int cLightning = 0;
		int cFruit = 0;

		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				Gem* foundGem = &gems[x][y];
				if (foundGem->color != gem.color)
				{
					// TODO: Locked and doom gems
					continue;
				}
				if (foundGem->Is(GemFlags::FRUIT))
					cFruit++;
				else if (foundGem->Is(GemFlags::LIGHTNING))
					cLightning++;
				else if (foundGem->Is(GemFlags::FLAME))
					cFlame++;
				else
					cRegular++;

				DestroyGem(foundGem->pos);
			}
		}

		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_NORMAL * cRegular, "SPECIAL_KILL_FRUIT_PERGEM_NORMAL");
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_FLAME * cFlame, "SPECIAL_KILL_FRUIT_PERGEM_FLAME");
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING * cLightning, "SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING");
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_FRUIT * cFruit, "SPECIAL_KILL_FRUIT_PERGEM_FRUIT");


	}
	else if (gem.Is(GemFlags::DOOM))
	{
		score.AddScoreNoMultiplier(SCORES::DOOM_DESTROY);
		if (gem.count == this->lowestBomb)
		{
			matchResultFlags |= MATCHRESULT_DEFUSED_DANGER;
		}
		matchResultFlags |= MATCHRESULT_DEFUSED | MATCHRESULT_KILLDOOM;
	}
	else if (gem.Is(GemFlags::LOCKED))
	{
		score.AddScore(SCORES::LOCK_DESTROY, "LOCK_DESTROY");
	}
	else if (gem.Is(GemFlags::BOMB))
	{
		score.AddScoreNoMultiplier(SCORES::BOMB_DESTROY);
		if (gem.count == this->lowestBomb)
		{
			matchResultFlags |= MATCHRESULT_DEFUSED_DANGER;
		}
		matchResultFlags |= MATCHRESULT_DEFUSED;
	}

	if (gem.locking)
	{
		score.AddScore(SCORES::LOCKING_DESTROY, "LOCKING_DESTROY");
	}

}

int Board::DestroyRow(int row)
{
	if (row < 0 || row >= 8) return 0;
	int count = 0;
	for (int x = 0; x < 8; x++)
	{
		Gem* gem = &gems[x][row];
		if (gem->color != GemColor::EMPTY)
		{
			DestroyGem({x, row});
			count++;
		}
	}
	return count;
}

int Board::DestroyCol(int col)
{
	if (col < 0 || col >= 8) return 0;
	int count = 0;
	for (int y = 0; y < 8; y++)
	{
		Gem* gem = &gems[col][y];
		if (gem->color != GemColor::EMPTY)
		{
			DestroyGem({col, y});
			count++;
		}
	}
	return count;
}

int Board::DestroyRadius(Vec2 pos, int radius)
{
	int ox = pos.x - radius / 2;
	int oy = pos.y - radius / 2;
	int count = 0;
	for (int x = ox; x < ox + radius; x++)
	{
		for (int y = oy; y < oy + radius; y++)
		{
			if (x >= 0 && x < 8 && y >= 0 && y < 8)
			{
				Gem* gem = &gems[x][y];
				if (gem->color != GemColor::EMPTY)
				{
					DestroyGem({x, y});
					count++;
				}
			}
		}
	}
	return count;
}

/// <summary>
/// Finds the gem closest to the center of a run that can be upgraded.
/// </summary>
/// <param name="run"></param>
/// <returns></returns>
Gem* Board::HopGems(Match* run)
{
	int size = run->match_size;
	Gem** gems = run->match;

	for (int i = 0; i < size; i++)
	{
		if (gems[i]->rotated && gems[i]->flags == GemFlags::NONE)
			return gems[i];
	}

	int start = ceil((double)size / 2) - 1;
	int def = floor((double)size / 2);
	int mult = (size % 2 == 0) ? 1 : -1;

	for (int i = 0; i < size; i++)
	{
		int idx = start + HOP_LOOKUP[i] * mult;
		if (gems[idx]->flags == GemFlags::NONE)
			return gems[idx];
	}

	return gems[def];
}

void Board::Collapse()
{

	for (int x = 0; x < 8; x++)
	{
		int stackHeight = 0;
		for (int y = 7; y >= 0; y--)
		{
			Gem* gem = &gems[x][y];
			if (gem->color != GemColor::EMPTY)
			{
				gem->pos.x = x;
				gem->pos.y = 7 - stackHeight;
				gems[x][7 - stackHeight++] = *gem;
			}
		}
		for (int y = 0; y <= 7 - stackHeight; y++)
		{
			Gem* gem = &gems[x][y];
			gem->color = GemColor::EMPTY;
			gem->flags = GemFlags::NONE;
			gem->pos.x = x;
			gem->pos.y = y;
		}
	}

}

void Board::FillRandomly(bool forceNoMove)
{
	printf("start fill\n");
	char flags[64] {};
	do
	{
		printf("> here's an attempt\n");
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				if (gems[x][y].color == GemColor::EMPTY || flags[x + y*8])
				{
					gems[x][y].color = (GemColor)(rand() % 7);
					flags[x + y * 8] = 1;
				}
			}
		}
	} while ((forceNoMove && ContainsMatch()) || !ContainsPossibleMatch());
	printf("filled successfully\n");

}
