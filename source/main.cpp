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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "boardwrapper.h"

// raygui embedded styles
// NOTE: Included in the same order as selector
#define MAX_GUI_STYLES_AVAILABLE   1       // NOTE: Included light style
#include "styles/style_dark.h"              // raygui style: dark

int main() 
{
	InitWindow(1152, 1152, "raygui - controls test suite");
	SetTargetFPS(60);
	GuiLoadStyleDark();

	bool showMessageBox = false;

	while (!WindowShouldClose())
	{
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
			
			Vector2 mouse_pos;
			draw_board(NULL, &mouse_pos);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}