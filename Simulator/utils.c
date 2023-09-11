#include "utils.h"

long long CreateLLMask(int start_bit, int end_bit)
{
	return ((1LL << (end_bit - start_bit)) - 1LL) << start_bit;
}

int ValueFitsInPrecision(long long value, int precision)
{
	return (-1LL << (precision - 1)) <= value && value < (1LL << precision);
}
