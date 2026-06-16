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
	_matches_s = 0;
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

void Board::Rotate(Vector2 pos)
{
	int x = pos.x;
	int y = pos.y;
	Gem buffer[4] = {gems[x][y], gems[x + 1][y], gems[x + 1][y + 1], gems[x][y + 1]};

	buffer[0].rotated = 1;
	buffer[1].rotated = 1;
	buffer[2].rotated = 1;
	buffer[3].rotated = 1;
	gems[x][y] = buffer[3].Move(x, y);
	gems[x + 1][y] = buffer[0].Move(x+1, y);
	gems[x + 1][y + 1] = buffer[1].Move(x+1, y+1);
	gems[x][y + 1] = buffer[2].Move(x,y+1);
}

void Board::AntiRotate(Vector2 pos)
{
	int x = pos.x;
	int y = pos.y;
	Gem buffer[4] = {gems[x][y], gems[x + 1][y], gems[x + 1][y + 1], gems[x][y + 1]};
	gems[x][y] = buffer[1].Move(x, y);
	gems[x + 1][y] = buffer[2].Move(x + 1, y);
	gems[x + 1][y + 1] = buffer[3].Move(x + 1, y + 1);
	gems[x][y + 1] = buffer[0].Move(x, y + 1);
}

int Board::RunMatch()
{
	for (int i = 0; i < 64; i++)
	{
		memset((void*)gemUpgrades, 0, sizeof(char) * 64);

		std::vector<Match*> matches, crossMatches;
		int matchCount = GetMatches(&matches, &crossMatches);
		if (matchCount == 0)
			break;
		score.ScoreCascade();

		int matchColorCount[7] = {0, 0, 0, 0, 0, 0, 0};

		for (Match* match : crossMatches)
		{
			Gem newGem = match->Get(0)->Copy();
			newGem.flags = GemFlags::FLAME;

			matchColorCount[(int)match->Get(0)->color]++;
			score.ScoreMatch(3);
			score.ScoreMatch(3);
			for (int i = 0; i < match->size; i++)
			{
				DestroyGem(match->Get(i)->pos);
			}
			gems[newGem.pos.x][newGem.pos.y] = newGem;
			gemUpgrades[newGem.pos.x][newGem.pos.y] = 1;
		}

		for (Match* match : matches)
		{
			score.ScoreMatch(match->size);
			matchColorCount[(int)match->Get(0)->color]++;

			int length = match->size;
			if (length >= 4)
			{
				Gem* picked = HopGems(match);
				if (length == 4)
					picked->flags = (GemFlags)(picked->flags | GemFlags::FLAME);
				else if (length == 5)
					picked->flags = (GemFlags)(picked->flags | GemFlags::LIGHTNING);
				else
					picked->flags = (GemFlags)((GemFlags)(picked->flags | GemFlags::LIGHTNING) | GemFlags::FLAME);
				gemUpgrades[picked->pos.x][picked->pos.y] = 1;
				
			}

			for (int i = 0; i < match->size; i++)
			{
				DestroyGem(match->Get(i)->pos);
			}
		}

		// bonus
		if (bonus.index < 4)
		{
			// allow going from 3rd bonus to 1st if 1st was matched
			if (matchColorCount[(int)bonus.colors[bonus.index]] == 0)
			{
				bonus.index = 0;
			}

			for (int i = bonus.index; i < 4; i++)
			{
				GemColor neededColor = bonus.colors[i];
				if (matchColorCount[(int)neededColor] > 0)
				{
					bonus.index++;
					matchColorCount[(int)neededColor]--;
					score.AddScore((i+1) * 200);
					if (bonus.index == 4)
					{
						if (bonus.fruitBonus)
							score.AddScore(SCORES::SPECIAL_MEGAFRUIT);
					}
				}
				else
				{
					break;
				}
			}
		}

		Collapse();
	}
	int finalScore = score.score;
	score.Reset();

	return finalScore/2;
}

int Board::GetMatches(std::vector<Match*>* matchesOut, std::vector<Match*>* crossMatchesOut)
{
	int matchCount = 0;
	// get rows and columns
	Gem* rows[8][8];
	Gem* cols[8][8];
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			rows[y][x] = &gems[x][y];
		}
	}
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			cols[x][y] = &gems[x][y];
		}
	}

	// find the matches
	for (int row = 0; row < 8; row++)
	{
		matchCount += FindMatchesInRun(rows[row]);
	}
	for (int col = 0; col < 8; col++)
	{
		matchCount += FindMatchesInRun(cols[col]);
	}

	// find intersections
	std::map<Gem*, Match*> gemMap;
	std::vector<Intersection> intersections;
	int regularMatchCount = _matches_s;
	for (int i = 0; i < regularMatchCount; i++)
	{
		Match* match = _matches + i;

		if (match->size != 3) continue;
		for (int j = 0; j < match->size; j++)
		{
			Gem* gem = match->Get(j);
			if (gemMap.contains(gem))
			{
				intersections.push_back({gemMap[gem], match, gem});
			}
			else
			{
				gemMap[gem] = match;
			}
		}
	}


	// prune intersections
	for (Intersection inter : intersections)
	{
		regularMatchCount -= 2;
		matchCount--;
		inter.match1->Erase(inter.gem);
		inter.match2->Erase(inter.gem);

		Match* interMatch = _matches + _matches_s++;
		interMatch->size = 0;
		interMatch->Push(inter.gem);

		for (int i = 0; i < inter.match1->size; i++)
		{
			interMatch->Push(inter.match1->Get(i));
		}
		for (int i = 0; i < inter.match2->size; i++)
		{
			interMatch->Push(inter.match2->Get(i));
		}
		crossMatchesOut->push_back(interMatch);

		Match match1 = *inter.match1;
		Match match2 = *inter.match2;



		foundMatches.Erase(match1);
		foundMatches.Erase(match2);
	}

	for (int i = 0; i < regularMatchCount; i++)
	{
		matchesOut->push_back(foundMatches.Get(i));
	}

	return matchCount;
}

/*
* optimize me :)
*/
int Board::FindMatchesInRun(Gem* run[8], ArrayPtr<Match>* matchesOut)
{
	const int MINIMUM = 3;
	int matchCount = 0;

	Match currentMatch;
	currentMatch.size = 0;
	//Match* currentMatch = matchesOut->Allocate();
	//currentMatch->size = 0;


	for (int i = 0; i < 8; i++)
	{
		Gem* gem = run[i];
		if (gem->color != GemColor::EMPTY &&
			(currentMatch.size == 0 || currentMatch.Get(0)->color == gem->color && gem->color != GemColor::COAL))
		{
			currentMatch.Push(gem);
		}
		else
		{
			if (currentMatch.size >= MINIMUM)
			{
				//currentMatch = matchesOut->Allocate();
				*matchesOut->Allocate() = currentMatch;
				matchCount++;
			}
			currentMatch.size = 0;
			if (gem->color != GemColor::EMPTY)
			{
				currentMatch.Push(gem);
			}
		}

	}

	if (currentMatch.size >= MINIMUM)
	{
		*matchesOut->Allocate() = currentMatch;
		matchCount++;
	}
	
	return matchCount;



	/*
	const int MINIMUM = 3;
	int matchCount = 0;

	Match currentMatch;
	for (int i = 0; i < 8; i++)
	{
		Gem* gem = run[i];
		if (gem->color != GemColor::EMPTY &&
			(currentMatch.size() == 0 || currentMatch[0]->color == gem->color && gem->color != GemColor::COAL))
		{
			currentMatch.push_back(gem);
		}
		else
		{
			if (currentMatch.size() >= MINIMUM)
			{
				matchesOut->push_back(currentMatch);
				matchCount++;
			}

			currentMatch.clear();
			if (gem->color != GemColor::EMPTY)
			{
				currentMatch.push_back(gem);
			}
		}

	}

	if (currentMatch.size() >= MINIMUM)
	{
		matchesOut->push_back(currentMatch);
		matchCount++;
	}
	return matchCount;
	*/
}

int Board::GetGemMatchCountFromMove(Vector2 pos)
{
	return 0;
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

void Board::DestroyGem(Vector2 pos)
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
		score.AddScore(SCORES::SPECIAL_KILL_FLAME);
		DestroyRadius(gem.pos, 3);
	}
	else if (gem.Is(GemFlags::LIGHTNING) && !gem.Is(GemFlags::FLAME))
	{
		int count = 1;
		score.AddScore(SCORES::SPECIAL_KILL_LIGHTNING);
		count += DestroyRow(gem.pos.y);
		count += DestroyCol(gem.pos.x);
		score.AddScore((int)SCORES::SPECIAL_KILL_LIGHTNING_PERGEM * count);
	}
	else if (gem.Is(GemFlags::LIGHTNING) && gem.Is(GemFlags::FLAME))
	{
		int count = 1;
		score.AddScore(SCORES::SPECIAL_KILL_LIGHTNING);
		count += DestroyRow(gem.pos.y - 1);
		count += DestroyRow(gem.pos.y);
		count += DestroyRow(gem.pos.y + 1);

		count += DestroyCol(gem.pos.x - 1);
		count += DestroyCol(gem.pos.x);
		count += DestroyCol(gem.pos.x + 1);
		score.AddScore((int)SCORES::SPECIAL_KILL_LIGHTNING_PERGEM * count);
	}
	else if (gem.Is(GemFlags::FRUIT))
	{
		score.AddScore(SCORES::SPECIAL_KILL_FRUIT);
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

		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_NORMAL * cRegular);
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_FLAME * cFlame);
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING * cLightning);
		score.AddScore((int)SCORES::SPECIAL_KILL_FRUIT_PERGEM_FRUIT * cFruit);

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

int Board::DestroyRadius(Vector2 pos, int radius)
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
	int size = run->size;

	for (int i = 0; i < size; i++)
	{
		if (run->Get(i)->rotated && run->Get(i)->flags == GemFlags::NONE)
			return run->Get(i);
	}

	int start = ceil((double)size / 2) - 1;
	int def = floor((double)size / 2);
	int mult = (size % 2 == 0) ? 1 : -1;

	for (int i = 0; i < size; i++)
	{
		int idx = start + HOP_LOOKUP[i] * mult;
		if (run->Get(idx)->flags == GemFlags::NONE)
			return run->Get(idx);
	}

	return run->Get(def);
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
				gems[x][7 - stackHeight++] = *gem;
			}
		}
		for (int y = 0; y <= 7 - stackHeight; y++)
		{
			Gem* gem = &gems[x][y];
			gem->color = GemColor::EMPTY;
			gem->flags = GemFlags::NONE;
		}
	}

}
