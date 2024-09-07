#pragma once
#include <math.h>
#include <iostream>

enum TIMING_INFO_TYPE
{
	LOCK,
	BOMB,
	COAL,
	DOOM
};

inline const char* TIMING_INFO_NAMES[] = {
	"LOCK",
	"BOMB",
	"COAL",
	"DOOM"
};

class TimingInfo
{

public:
	int count_before_next = -1;
	bool trigger = false;
	double multiplier = 1.0;

	int first_level;
	int first_change_level;
	int base;
	int value_base;
	int levels_between_change;
	int change;
	int value_change;
	int min;
	int max;
	int value_min;
	int value_max;
	int turns_between_change;
	bool turn_based;
	double chance_pow;

	TimingInfo();
	TimingInfo(TIMING_INFO_TYPE type, int level, int move_count);
	~TimingInfo();

	void setup_bomb();
	void setup_lock();
	void setup_coal();
	void setup_doom();

	bool attempt_trigger_turnbased(int level, int move_count);
	bool attempt_trigger(int level, int move_count);
	bool roll(int level, int move_count);
	int calc_base(int level, int move_count);
	int calc_value_base(int level, int move_count);

	int reset(int level, int move_count);

private:
	int calc_base_generic(int base, int change, int min, int max, int level, int move_count);
	TIMING_INFO_TYPE timing_type;
};

