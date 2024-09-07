#include "boardwrapper.h"

void load_sprites()
{

	printf("Loading sprites...");

	memset((void*)GEM_SPRITES, 0, sizeof(Texture2D) * 9);

	GEM_SPRITES[0] = LoadTexture("../res/gem_0.png");
	GEM_SPRITES[1] = LoadTexture("../res/gem_1.png");
	GEM_SPRITES[2] = LoadTexture("../res/gem_2.png");
	GEM_SPRITES[3] = LoadTexture("../res/gem_3.png");
	GEM_SPRITES[4] = LoadTexture("../res/gem_4.png");
	GEM_SPRITES[5] = LoadTexture("../res/gem_5.png");
	GEM_SPRITES[6] = LoadTexture("../res/gem_6.png");
	GEM_SPRITES[7] = LoadTexture("../res/gem_7.png");

	for (int i = 0; i < 8; i++)
	{
		while(!IsTextureReady(GEM_SPRITES[i]));
	}

	printf("loaded.\n");

}

void unload_sprites()
{
	for (int i = 0; i < 8; i++)
	{
		UnloadTexture(GEM_SPRITES[i]);
	}
}

bool draw_board(Board* board, Vector2* cursor_pos)
{

	if (board == NULL)
	{
		throw new std::exception();
	}

	Rectangle board_rect = {ORIGIN_X, ORIGIN_Y, GRID_W * CELL_SIZE, GRID_H * CELL_SIZE};
	DrawRectangleRec(board_rect, COLOR_BOARD_BG);

	double cur_time = GetTime();

	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			int i = y * GRID_W + x;

			Rectangle rect = {(float)ORIGIN_X + CELL_SIZE * x, (float)ORIGIN_Y + CELL_SIZE * y, CELL_SIZE, CELL_SIZE};
			Vector2 center = {rect.x + rect.width/2, rect.y + rect.height/2};
			DrawRectangleLinesEx(rect, 1, COLOR_BOARD_LINE);

			// draw the gem fool
			Gem* gem = &(board->gems[x][y]);
			if (gem != NULL && gem->color != GemColor::EMPTY)
			{
				bool is_flame = gem->Is(GemFlags::FLAME);
				bool is_lightning = gem->Is(GemFlags::LIGHTNING);
				bool is_bomb = gem->Is(GemFlags::BOMB);
				bool is_locked = gem->Is(GemFlags::LOCKED);
				bool is_doom = gem->Is(GemFlags::DOOM);
				bool is_locking = gem->locking;
				bool is_dooming = gem->Is(GemFlags::DOOMSPAWN);

				if (is_flame && !is_lightning)
				{
					DrawRectangleRec(rect, {255, 128, 0, 96});
				}
				else if (!is_flame && is_lightning)
				{
					DrawRectangleRec(rect, {255, 255, 255, 96});
				}
				else if (is_flame && is_lightning)
				{
					DrawRectangleRec(rect, {255, 0, 255, 128});
				}
				else if (is_bomb)
				{
					DrawRectangleRec(rect, {0, 0, 0, 200});
				}

				if (is_dooming)
				{
					Color bg = {255, 100, 100, 255};
					bg.a = sin(cur_time*PI*4) * 127.5 + 127.5;
					DrawRectangleRec(rect, bg);
				}


				Texture2D tex = GEM_SPRITES[(int)gem->color];
				//DrawTextureEx(GEM_SPRITES[(int)gem->color], {rect.x, rect.y}, 0, 0.5, {255,255,255,255});

				if (is_bomb)
				{
					DrawTextureEx(GEM_SPRITES[(int)gem->color], {rect.x + rect.width/2 - rect.width/3, rect.y + rect.height/2 - rect.width/3}, 0, 0.3333, {255,255,255,255});

					char bomb_text[8];
					sprintf(bomb_text, "%2i", gem->count);
					int text_width = MeasureText(bomb_text, 32);
					DrawText(bomb_text, center.x - text_width/2, center.y - 16, 32, {255, 255, 255, 255});
				}
				else if (is_doom)
				{
					DrawRectangleRec(rect, {255, 100, 100, 255});
					char bomb_text[8];
					sprintf(bomb_text, "%2i", gem->count);
					int text_width = MeasureText(bomb_text, 64);
					DrawText(bomb_text, center.x - text_width/2, center.y - 32, 64, {255, 255, 255, 255});
				}
				else
				{
					DrawTextureEx(GEM_SPRITES[(int)gem->color], {rect.x, rect.y}, 0, 0.5, {255,255,255,255});
				}

				Color grey = {150, 150, 150, 255};
				if (is_locking)
				{
					grey.a = sin(cur_time*PI*4) * 127.5 + 127.5;
					DrawLineEx({rect.x, rect.y}, {rect.x + rect.width, rect.y + rect.height}, 4, grey);
					DrawLineEx({rect.x + rect.width, rect.y}, {rect.x, rect.y + rect.height}, 4, grey);
				}
				else if (is_locked)
				{
					DrawLineEx({rect.x, rect.y}, {rect.x + rect.width, rect.y + rect.height}, 4, grey);
					DrawLineEx({rect.x + rect.width, rect.y}, {rect.x, rect.y + rect.height}, 4, grey);
					DrawRectangleLinesEx(rect, 4, grey);
				}

			}

		}
	}

	char buff[64];
	sprintf(buff, "Level: %d", board->level);
	DrawText(buff, (int)board_rect.x, (int)board_rect.y + board_rect.height + 2, 24, {255,255,255,255});

	sprintf(buff, "LOCK: %d  BOMB: %d  COAL: %d  DOOM: %d",
		board->timing_lock.count_before_next,
		board->timing_bomb.count_before_next,
		board->timing_coal.count_before_next,
		board->timing_doom.count_before_next);

	DrawText(buff, (int)board_rect.x, (int)board_rect.y + board_rect.height + 26, 24, {255,100,100,255});

	Vector2 mouse_pos = GetMousePosition();


	if (board->gameOver)
	{
		DrawText("GAME OVER", ORIGIN_X, ORIGIN_Y, 32, {255,0,0,255});

		
		for (int x = 0; x < GRID_W; x++)
		{
			for (int y = 0; y < GRID_H; y++)
			{
				int i = y * GRID_W + x;

				Rectangle rect = {(float)ORIGIN_X + CELL_SIZE * x, (float)ORIGIN_Y + CELL_SIZE * y, CELL_SIZE, CELL_SIZE};

				// draw the gem fool
				Gem* gem = &(board->gems[x][y]);
				if ((gem->Is(GemFlags::BOMB) || gem->Is(GemFlags::DOOM)) && gem->count <= 0)
				{
					DrawRectangleLinesEx(rect, 8, {255, 0, 0, 255});
				}
			}
		}

		return false;
	}

	if (CheckCollisionPointRec(mouse_pos, board_rect))
	{
		Vector2 grid_pos;
		grid_pos.x = max(0, min(GRID_W-2, (int)floorf((mouse_pos.x - board_rect.x - CELL_SIZE/2) / CELL_SIZE)));
		grid_pos.y = max(0, min(GRID_H-2, (int)floorf((mouse_pos.y - board_rect.y - CELL_SIZE/2) / CELL_SIZE)));

		bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

		DrawRing({board_rect.x + grid_pos.x * CELL_SIZE + CELL_SIZE, board_rect.y + grid_pos.y * CELL_SIZE + CELL_SIZE},
			CELL_SIZE - 2, CELL_SIZE + 2, 0, 360, 32, {255,0,255,255});

		/* DrawRectangleLinesEx({
			board_rect.x + grid_pos.x * CELL_SIZE,
			board_rect.y + grid_pos.y * CELL_SIZE,
			CELL_SIZE*2,
			CELL_SIZE*2
		}, 4, {255,0,255,255}); */
		cursor_pos->x = grid_pos.x;
		cursor_pos->y = grid_pos.y;
		if (clicked)
		{
			return true;
		}

	}

	return false;

}
