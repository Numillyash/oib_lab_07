// Лабораторная работа №7
// Вариант: A + LZW 

#define _CRT_SECURE_NO_WARNINGS

#include "Arithmetic_coding.h"


int main()
{
	unsigned char message[] = { "babbc" };

	double a, b;

	find_interval(&a,&b, message);
}