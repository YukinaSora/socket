#include "utils.h"

#include "common.h"

void* try_malloc(unsigned long long size)
{
	void* ptr = malloc(size);
	if (ptr == nullptr)
	{
		fprintf(stderr, "Bad malloc.\n");
		exit(EXIT_FAILURE);
	}
	return ptr;
}

int str_to_int(const char* str)
{
	int result = 0;
	int sign = 1;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	while (*str != '\0')
	{
		if (*str >= '0' && *str <= '9')
		{
			result = result * 10 + *str - '0';
		}
		else
		{
			break;
		}
		str++;
	}
	return result * sign;
}