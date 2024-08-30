/*******************************************************************************************
*
*   raygui - style selector
*
*   DEPENDENCIES:
*       raylib 4.5          - Windowing/input management and drawing
*       raygui 3.5          - Immediate-mode GUI controls with custom styling and icons
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2016-2024 Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"
#include "Board.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "boardwrapper.h"

// raygui embedded styles
// NOTE: Included in the same order as selector
#define MAX_GUI_STYLES_AVAILABLE   1       // NOTE: Included light style
#include "styles/style_dark.h"              // raygui style: dark

int main() 
{

	const GemColor colors[] = {
		GemColor::RED, GemColor::ORANGE, GemColor::WHITE, GemColor::RED, GemColor::BLUE, GemColor::YELLOW, GemColor::ORANGE, GemColor::ORANGE,
		GemColor::YELLOW, GemColor::PURPLE, GemColor::GREEN, GemColor::RED, GemColor::GREEN, GemColor::RED, GemColor::ORANGE, GemColor::ORANGE,
		GemColor::GREEN, GemColor::RED, GemColor::WHITE, GemColor::WHITE, GemColor::RED, GemColor::WHITE, GemColor::BLUE, GemColor::BLUE,
		GemColor::RED, GemColor::RED, GemColor::GREEN, GemColor::PURPLE, GemColor::YELLOW, GemColor::GREEN, GemColor::BLUE, GemColor::PURPLE,

		GemColor::ORANGE, GemColor::YELLOW, GemColor::WHITE, GemColor::RED, GemColor::GREEN, GemColor::PURPLE, GemColor::WHITE, GemColor::YELLOW,
		GemColor::GREEN, GemColor::YELLOW, GemColor::WHITE, GemColor::GREEN, GemColor::BLUE, GemColor::BLUE, GemColor::PURPLE, GemColor::RED,
		GemColor::ORANGE, GemColor::RED, GemColor::ORANGE, GemColor::ORANGE, GemColor::PURPLE, GemColor::GREEN, GemColor::YELLOW, GemColor::PURPLE,
		GemColor::GREEN, GemColor::GREEN, GemColor::ORANGE, GemColor::YELLOW, GemColor::PURPLE, GemColor::WHITE, GemColor::GREEN, GemColor::YELLOW
	};

	SetConfigFlags(FLAG_MSAA_4X_HINT);

	InitWindow(1152, 1152, "raygui - controls test suite");
	load_sprites();
	SetTargetFPS(60);
	GuiLoadStyleDark();

	bool showMessageBox = false;

	srand(14);
	Board* board = new Board();
	board->FillRandomly(true);


	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			board->gems[x][y].color = colors[y*8 + x];


			if (x == 1 && y == 6)
				board->gems[x][y].flags = (GemFlags)(board->gems[x][y].flags | GemFlags::FLAME);
				
			if (x == 2 && y == 4)
				board->gems[x][y].flags = (GemFlags)(board->gems[x][y].flags | GemFlags::LIGHTNING);
		}
	}

	while (!WindowShouldClose())
	{
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();


			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			
			Vector2 mouse_pos;
			if (IsKeyPressed(KEY_T))
				board->gems[(int)mouse_pos.x][(int)mouse_pos.y].flags = (GemFlags)(board->gems[(int)mouse_pos.x][(int)mouse_pos.y].flags | GemFlags::LIGHTNING);
			if (draw_board(board, &mouse_pos))
			{
				srand(14);
				board->Rotate({(int)mouse_pos.x, (int)mouse_pos.y});
				board->RunMatch(true);
			}

		EndDrawing();
	}

	CloseWindow();
	unload_sprites();

	delete board;

	return 0;
}