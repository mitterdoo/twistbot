#pragma once
#include <stdexcept>

typedef struct Vector2 {
	int x;
	int y;
} Vector2;

template <typename T> class ArrayPtr
{
private:
	T ptr[];

public:
	ArrayPtr(T pointer[], int maxSize, int size = 0);

	int size;
	int max_size;

	inline void Push(T item)
	{
		if (size >= max_size)
			throw new std::runtime_error("Array at max size!");
		ptr[size++] = item;
	}

	inline T* Get(int index)
	{
		return &ptr[index];
	}

	void Erase(T what)
	{
		char found = 0;
		for (int i = 0; i < max_size; i++)
		{
			if (memcmp((void*)(ptr + i), (void*)&what, sizeof(T)) == 0)
				found = 1;
			if (found && i < max_size-1)
				ptr[i] = ptr[i + 1];
		}
		size -= found;
	}

	T* Allocate()
	{
		if (size >= max_size)
			throw new std::runtime_error("Array at max size!");
		return &ptr[size++];
	}
};

template <typename T> ArrayPtr<T>::ArrayPtr(T pointer[], int maxSize, int size)
{
	ptr = pointer;
	max_size = maxSize;
	this->size = size;
}

