#pragma once
#include <iostream>
#include <stdexcept>
#include <windows.h>
#include <WinUser.h>
#include "Board.h"
#include "Gem.h"

constexpr auto MEM_STATIC_BOARD = 0x0085AEF4;

constexpr auto MEM_OFFSET_SCORE	= 0x0168;
constexpr auto MEM_OFFSET_LEVEL = 0x0158;
constexpr auto MEM_OFFSET_LEVEL_SCORE = 0x0200;
constexpr auto MEM_OFFSET_MULTIPLIER = 0x21D0;
constexpr auto MEM_OFFSET_COMBO = 0x0224;
constexpr auto MEM_OFFSET_BOARD = 0x1338;

constexpr auto MEM_OFFSET_GEM_COLOR = 0x08;
constexpr auto MEM_OFFSET_GEM_SCREEN_X = 0x0C;
constexpr auto MEM_OFFSET_GEM_SCREEN_Y = 0x10;
constexpr auto MEM_OFFSET_GEM_GRID_X = 0x20;
constexpr auto MEM_OFFSET_GEM_GRID_Y = 0x24;
constexpr auto MEM_OFFSET_GEM_FLAGS = 0x84;
constexpr auto MEM_OFFSET_GEM_COUNT = 0x100;
constexpr auto MEM_OFFSET_GEM_LOCKING = 0x4FC;

constexpr auto MEM_OFFSET_CANMOVE = 0x22B8;
constexpr auto MEM_OFFSET_REPLAYWIDGET = 0x2624;
constexpr auto MEM_OFFSET_REPLAYWIDGET_VISIBLE = 0x54;
constexpr auto MEM_OFFSET_BONUS_ADDR = 0x264C;
constexpr auto MEM_OFFSET_BONUS_MEGA = 0x265C;
constexpr auto MEM_OFFSET_BONUS_IDX = 0x2660;
constexpr auto MEM_OFFSET_CURSOR_X = 0x1C10;
constexpr auto MEM_OFFSET_CURSOR_Y = 0x1C14;

constexpr auto MEM_OFFSET_GOODTUBE = 0x2194;

constexpr auto MEM_OFFSET_GOODTUBE_BONUSSTATE = 0x3B0;
constexpr auto MEM_OFFSET_GOODTUBE_BONUSTIMER = 0x3B4;

class BejGame
{
public:
	BejGame();
	~BejGame();

	void ReadMemory(SIZE_T address, SIZE_T count, void* buffer);
	void WriteMemory(SIZE_T address, SIZE_T count, void* buffer);

	char ReadChar(SIZE_T address);
	uint32_t ReadUInt(SIZE_T address);
	uint32_t ReadUInt_WaitForNonZero(SIZE_T address);
	float ReadFloat(SIZE_T address);

	void WriteUInt(SIZE_T address, uint32_t value);
	void SetGemFlag(Vector2 pos, GemFlags flag);
	void SetGemColor(Vector2 pos, GemColor color);

	void WaitForStationaryGems();
	void WaitForBonusComplete();

	/// <summary>
	/// Clicks the OS mouse at the given coordinates of the window.
	/// </summary>
	/// <param name="pos">Coordinates for the click.</param>
	void Click(Vector2 pos);

	Board* FetchBoard();
	Vector2 GetCursor();

private:
	HANDLE hProc;
	HWND hWnd;
};
