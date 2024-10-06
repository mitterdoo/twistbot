#pragma once

#include "Board.h"
#include "Gem.h"
#include "raylib.h"

#define COLOR_BOARD_BG {32,32,32,255}
#define COLOR_BOARD_LINE {48, 48, 48, 255}

const Color GEM_COLORS[] = {
	{255, 64, 64, 255},
	{255, 255, 255, 255},
	{64, 255, 64, 255},
	{255, 255, 64, 255},
	{180, 64, 255, 255},
	{255, 127, 64, 255},
	{64, 64, 255, 255},
	{100, 100, 100, 255},
	{0,0,0,0}
};

#define CELL_SIZE 128
#define GRID_W 8
#define GRID_H 8
#define GEM_PADDING 20

#define ORIGIN_X 350
#define ORIGIN_Y 32

inline Texture2D GEM_SPRITES[9];

void load_sprites();
void unload_sprites();

/// @brief Draws the board
/// @param board 
/// @param mouse_click_pos Output for mouse click position
/// @return Whether the mouse was clicked
bool draw_board(Board* board, Vector2* mouse_click_pos);
