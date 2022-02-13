#include "Arithmetic_coding.h"
#include <string.h>

int existing_symbols[26];
double existing_symbols_size[26];
double existing_symbols_left[26];
double existing_symbols_right[26];

void find_interval(double* left, double* right, char* string)
{
	int str_len = strlen(string);

	double curr_left, curr_size, new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;

	for (int i = 0; i < 26; i++)
	{
		existing_symbols[i] = 0;
		existing_symbols_size[i] = 0;
		existing_symbols_left[i] = 0;
		existing_symbols_right[i] = 0;
	}

	for (int i = 0; i < str_len; i++)
	{
		existing_symbols[(int)string[i] - (int)'a']++;
	}

	for (int i = 0; i < 26; i++)
	{
		if (existing_symbols[i])
		{
			existing_symbols_size[i] = (double) existing_symbols[i] / str_len;
			if (i)
			{
				existing_symbols_left[i] = new_left + new_size;
			}
			else
			{
				existing_symbols_left[i] = 0;
			}
			new_left = existing_symbols_left[i];
			new_size = existing_symbols_size[i];
			existing_symbols_right[i] = new_left + new_size;
#ifdef DEBUG
			printf("For symbol %c: left is %f, right is %f,  size is %f\n", (char)((int)'a' + i), existing_symbols_left[i], existing_symbols_right[i], existing_symbols_size[i]);
#endif // DEBUG
		}
	}

	for (int i = 0; i < str_len; i++)
	{
		int ind = (int)string[i] - (int)'a';
		curr_left = existing_symbols_left[ind];
		curr_size = existing_symbols_size[ind];
		new_left = prev_left + curr_left * prev_size;
		new_size = curr_size * prev_size;
		new_right = new_left + new_size;
		prev_size = new_size;
		prev_left = new_left;

	}
#ifdef DEBUG
	printf("Left is %f, size is %f, right is %f\n", new_left, new_size, new_right);
#endif // DEBUG
	int x = 1, degree = 1;
	double rand_chislo = 1;
	while (rand_chislo > new_left)
	{
		rand_chislo /= 2;
		degree++;
	}
	if (rand_chislo * 2 > new_right)
	{
		while (1)
		{
			while ((rand_chislo * (x+2)) < new_right)
			{
				x+=2;
			}
			if ((rand_chislo * x) > new_left)
			{
				break;
			}
			else
			{
				x = 1;
				rand_chislo /= 2;
				degree++;
			}
		}
	}
	printf("x = %d, degree = %d, number = %f\n", x, degree - 1, rand_chislo*x);
}


void coding_text()
{

}