#include "Arithmetic_coding.h"

int existing_symbols[ALPHABET_SIZE];
double existing_symbols_size[ALPHABET_SIZE];
double existing_symbols_left[ALPHABET_SIZE];
double existing_symbols_right[ALPHABET_SIZE];
double x = 0; int degree = 0; // for double x / (2^degree) number
double decode_number = 1, degree_minus;


void find_interval(char* string)
{
	int str_len = strlen(string);
	int i = 0; // iterator
	double curr_left, curr_size, new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;
	int ind; // index of current symbol
	double right, left; // interval borders that will be changed due sicle

	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		existing_symbols[i] = 0;
		existing_symbols_size[i] = 0;
		existing_symbols_left[i] = 0;
		existing_symbols_right[i] = 0;
	}

	for (i = 0; i < str_len; i++)
	{
		existing_symbols[(int)string[i] - (int)'a']++;
	}

	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		if (existing_symbols[i])
		{
			existing_symbols_size[i] = (double)existing_symbols[i] / str_len;
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
		}
	}

	for (int i = 0; i < str_len; i++)
	{
		ind = (int)string[i] - (int)'a';
		curr_left = existing_symbols_left[ind];
		curr_size = existing_symbols_size[ind];
		new_left = prev_left + curr_left * prev_size;
		new_size = curr_size * prev_size;
		new_right = new_left + new_size;
		prev_size = new_size;
		prev_left = new_left;
	}

	right = new_right;
	left = new_left;
	while (1)
	{
		if (0.5 > right)
		{
			decode_number /= 2;
			x = 2 * x + 0;
		}
		else if (0.5 < left)
		{
			decode_number /= 2;
			x = 2 * x + 1;
		}
		else if (0.5 == right)
		{
			degree_minus = 0.5;
			x = 2 * x + 0;
			degree++;
			decode_number /= 2;
			while (right - degree_minus < left) {
				degree_minus /= 2;
				x = 2 * x + 1;
				degree++;
				decode_number /= 2;
			}
			break;
		}
		else
		{
			x = 2 * x + 1;
			decode_number /= 2;
			degree++;
			break;
		}
		right *= 2; if (right >= 1) right -= 1;
		left *= 2; if (left >= 1) left -= 1;
		if (right == 0 && left == 0)
			break;
		degree++;
	}

	decode_number *= x;
}

void coding_text(char* msg)
{
	int str_len = (int)strlen(msg);
	char* message = (char*)malloc(str_len + 2);
	message[0] = '\0';
	strcat(message, msg);
	strcat(message, "{");

	find_interval(message);

	for (int i = 0; i < 27; i++)
	{
		if (existing_symbols[i])
		{
			printf("For symbol %c: left is %f, right is %f,  size is %f\n", (char)((int)'a' + i), existing_symbols_left[i], existing_symbols_right[i], existing_symbols_size[i]);
		}
	}
	printf("Decoding number\nx = %f, degree = %d, number = %f\n", x, degree, decode_number);
}

void decoding_text()
{
	double curr_left, curr_size, new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;
	char symb = ' ';

	printf("Text is: \n");
	while (symb != '{')
	{
		for (int i = 0; i < 27; i++)
		{
			if (existing_symbols[i])
			{
				curr_left = existing_symbols_left[i];
				curr_size = existing_symbols_size[i];
				new_left = prev_left + curr_left * prev_size;
				new_size = curr_size * prev_size;
				new_right = new_left + new_size;
				if (new_left <= decode_number && decode_number < new_right)
				{
					symb = (char)((int)'a' + i);
					prev_size = new_size;
					prev_left = new_left;
					break;
				}
			}
		}
		if (symb != '{')
			printf("%c", symb);
	}

}