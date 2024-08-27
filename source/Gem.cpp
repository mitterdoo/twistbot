#include <iostream>
#include "Gem.h"

Gem::Gem(GemColor color, GemFlags flags, Vec2 pos)
{
	this->color = color;
	this->flags = flags;
	this->pos = pos;
	pixelPos = { 0, 0 };
}

Gem::Gem(Vec2 pos)
{
	color = GemColor::EMPTY;
	flags = GemFlags::NONE;
	this->pos = pos;
	pixelPos = {0, 0};
}

Gem::Gem()
{
	color = GemColor::EMPTY;
	flags = GemFlags::NONE;
	pos = { 0, 0 };
	pixelPos = { 0, 0 };
}

Gem Gem::Copy()
{
	Gem gem = Gem(color, flags, pos);
	gem.pixelPos = pixelPos;

	return gem;
}


Gem::~Gem()
{
}


Gem Gem::Move(int x, int y)
{
	pos.x = x;
	pos.y = y;
	return *this;
}
