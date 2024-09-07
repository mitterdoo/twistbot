#include "TimingInfo.h"

TimingInfo::TimingInfo()
{

}

TimingInfo::TimingInfo(TIMING_INFO_TYPE type, int level, int move_count)
{
	switch (type)
	{
		case TIMING_INFO_TYPE::LOCK:
			setup_lock();
			break;

		case TIMING_INFO_TYPE::BOMB:
			setup_bomb();
			break;
		
		case TIMING_INFO_TYPE::COAL:
			setup_coal();
			break;

		case TIMING_INFO_TYPE::DOOM:
			setup_doom();
			break;

	}
	reset(level, move_count);
}

TimingInfo::~TimingInfo()
{

}

void TimingInfo::setup_bomb()
{
	timing_type = BOMB;
	first_level = 2;
	first_change_level = 3;
	base = 35;
	value_base = 20;
	levels_between_change = 1;
	change = -5;
	value_change = -1;
	min = 7;
	max = -1;
	value_min = 7;
	value_max = 20;
	turns_between_change = 0;
	turn_based = 0;
	chance_pow = 5.0;
	multiplier = 1.0;
}

void TimingInfo::setup_lock()
{
	timing_type = LOCK;
	first_level = 4;
	first_change_level = 5;
	base = 35;
	value_base = 1;
	levels_between_change = 1;
	change = -3;
	value_change = 1;
	min = 7;
	max = -1;
	value_min = -1;
	value_max = -1;
	turns_between_change = 0;
	turn_based = 1;
	chance_pow = 6.0;
	multiplier = 1.0;
}

void TimingInfo::setup_coal()
{
	timing_type = COAL;
	first_level = 3;
	first_change_level = 6;
	base = 100;
	value_base = 1;
	levels_between_change = 3;
	change = -1;
	value_change = 1;
	min = 35;
	max = -1;
	value_min = -1;
	value_max = -1;
	turns_between_change = 0;
	turn_based = false;
	chance_pow = 3.0;
	multiplier = 1.0;
}

void TimingInfo::setup_doom()
{
	timing_type = DOOM;
	first_level = 13;
	first_change_level = 14;
	base = 100;
	value_base = 13;
	levels_between_change = 1;
	change = -1;
	value_change = -3;
	min = 20;
	max = -1;
	value_min = 9;
	value_max = -1;
	turns_between_change = 0;
	turn_based = 0;
	chance_pow = 0.0;
	multiplier = 1.0;
}

bool TimingInfo::roll(int level, int move_count)
{
	int base = calc_base(level, move_count);

	count_before_next--;

	if (chance_pow == 0)
	{
		return (trigger = true);
	}

	double result = (double)(base - count_before_next) / base * multiplier;
	result = pow(result, chance_pow);

	double this_roll = (double)(rand() % RAND_MAX) / (double)RAND_MAX;

	trigger = result >= this_roll;
	if (trigger)
	{
		printf("TimingInfo::roll() proc (%s)\n", TIMING_INFO_NAMES[(int)timing_type]);
	}

	return trigger;

}

bool TimingInfo::attempt_trigger(int level, int move_count)
{
	if (level < first_level || first_level < 0)
		return false;
	
	if (trigger)
		return true;

	if (!turn_based)
		roll(level, move_count);

	return trigger;
}

bool TimingInfo::attempt_trigger_turnbased(int level, int move_count)
{
	if (level < first_level || first_level < 0)
		return false;
	
	if (turn_based)
		roll(level, move_count);

	return trigger;
}

int TimingInfo::reset(int level, int move_count)
{
	trigger = false;
	return (count_before_next = calc_base(level, move_count));
}

int TimingInfo::calc_base(int level, int move_count)
{
	return calc_base_generic(base, change, min, max, level, move_count);
}

int TimingInfo::calc_value_base(int level, int move_count)
{
	return calc_base_generic(value_base, value_change, value_min, value_max, level, move_count);
}

int TimingInfo::calc_base_generic(int base, int change, int min, int max, int level, int move_count)
{
	if (level < first_level || first_level < 0)
	{
		return -1;
	}

	int result = base;
	if (level >= first_change_level)
	{
		result += ((double)(level - first_change_level + 1))/ levels_between_change * change;
	}
	if (turns_between_change > 0)
	{
		result += (double)move_count / turns_between_change * change;
	}
	if (min >= 0 && min > result)
		result = min;
	
	if (max >= 0 && max < result)
		return max;
	
	return result;

}
