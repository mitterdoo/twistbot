#pragma once
#include "HandyTypes.hpp"
#include <fstream>

enum class GemColor {
	GEM_RED = 0,
	GEM_WHITE = 1,
	GEM_GREEN = 2,
	GEM_YELLOW = 3,
	GEM_PURPLE = 4,
	GEM_ORANGE = 5,
	GEM_BLUE = 6,
	COAL = 7,
	EMPTY = 8,
};

enum class GemFlags {
	NONE = 0x0000,		// 0
	FRUIT = 0x0001,		// 1
	SKULL = 0x0002,		// 2
	LOCKED = 0x0004,	// 3
	LIGHTNING = 0x0008, // 4
	FLAME = 0x0010,		// 5
	IMMOVABLE = 0x0020, // 6
	ICE = 0x0040,		// 7
	ANGEL = 0x0080,		// 8
	DOOM = 0x0100,		// 9		Doom gems will only have this flag--they don't have a combination of other flags like bomb or anything.
	BOMB = 0x0200,		// 10
	COAL = 0x0400,		// 11
	UNK = 0x0800,		// 12 (unknown)
	DOOMSPAWN = 0x1000  // 13 (of course it's 13)
};

inline int operator |(GemFlags a, GemFlags b)
{
	return static_cast<int>(a) | static_cast<int>(b);
}

inline int operator |(GemFlags a, int b)
{
	return static_cast<int>(a) | b;
}

inline int operator &(GemFlags a, GemFlags b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}

inline int operator &(GemFlags a, int b)
{
	return static_cast<int>(a) & b;
}

inline int operator ~(GemFlags a)
{
	return ~static_cast<int>(a);
}

class Gem
{
public:
	/// <summary>
	/// Creates a Gem with specified properties and position
	/// </summary>
	/// <param name="color"></param>
	/// <param name="flags"></param>
	/// <param name="pos"></param>
	Gem(GemColor color, GemFlags flags, Vec2 pos);

	/// <summary>
	/// Creates a GemColors::EMPTY gem at the position
	/// </summary>
	/// <param name="pos"></param>
	Gem(Vec2 pos);

	/// <summary>
	/// Creates a GemColors::EMPTY gem at (0, 0)
	/// </summary>
	Gem();
	~Gem();

	Gem Copy();
	inline int Is(GemFlags other)
	{
		return (this->flags & other);
	}

	GemColor color;
	GemFlags flags;
	std::string str;
	int rotated = 0;
	int count = 0;
	int locking = 0;

	Vec2 pos;
	Vec2 pixelPos;

	Gem Move(int x, int y);
	inline const char* StringColor() { return flags == GemFlags::NONE ? GEM_COLORS[(int)color] : GEM_COLORS_SPECIAL[(int)color]; }


	/*
	Name            FG  BG
	Black           30  40
	Red             31  41
	Green           32  42
	Yellow          33  43
	Blue            34  44
	Magenta         35  45
	Cyan            36  46
	White           37  47
	Bright Black    90  100
	Bright Red      91  101
	Bright Green    92  102
	Bright Yellow   93  103
	Bright Blue     94  104
	Bright Magenta  95  105
	Bright Cyan     96  106
	Bright White    97  107
	*/
	static const constexpr char* const GEM_COLORS[9] = {
		"\x1B[91m\xFE\x1B[0m ",
		"\x1B[97m\xFE\x1B[0m ",
		"\x1B[92m\xFE\x1B[0m ",
		"\x1B[93m\xFE\x1B[0m ",
		"\x1B[35m\xFE\x1B[0m ",
		"\x1B[33m\xFE\x1B[0m ",
		"\x1B[94m\xFE\x1B[0m ",
		"\x1B[97m+\x1B[0m ",
		"  "
	};
	static const constexpr char* const GEM_COLORS_SPECIAL[9] = {
		"\x1B[91m\xDB\x1B[0m ",
		"\x1B[97m\xDB\x1B[0m ",
		"\x1B[92m\xDB\x1B[0m ",
		"\x1B[93m\xDB\x1B[0m ",
		"\x1B[35m\xDB\x1B[0m ",
		"\x1B[33m\xDB\x1B[0m ",
		"\x1B[94m\xDB\x1B[0m ",
		"\x1B[97m+\x1B[0m ",
		"  "
	};

};
