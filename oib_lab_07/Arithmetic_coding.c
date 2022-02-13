#include "Arithmetic_coding.h"

int existing_symbols[27];
double existing_symbols_size[27];
double existing_symbols_left[27];
double existing_symbols_right[27];
double x = 0; int degree = 0; // for double x / (2^degree) number
double decode_number = 1;

void find_interval(char* string)
{
	int str_len = strlen(string);

	double curr_left, curr_size, new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;

	for (int i = 0; i < 27; i++)
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

	for (int i = 0; i < 27; i++)
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
		int ind = (int)string[i] - (int)'a';
		curr_left = existing_symbols_left[ind];
		curr_size = existing_symbols_size[ind];
		new_left = prev_left + curr_left * prev_size;
		new_size = curr_size * prev_size;
		new_right = new_left + new_size;
		prev_size = new_size;
		prev_left = new_left;
	}

	//0.01001000000101101111
	//0.010001011110000010110101
	//0.01001

	printf("new_right-new_left                 = %.60e\nnew_right-new_left (but in double) = %.60e\nnew_left                           = %.60e\nnew_right                          = %.60e\nnew_left+new_size                  = %.60e\n", new_right-new_left, new_size, new_left, new_right, new_left+new_size);
	double right = new_right, left = new_left;
	while (1)
	{
		if (0.5 >= right)
		{
			decode_number /= 2;
			x = 2 * x + 0;
		}
		else if (0.5 < left)
		{
			decode_number /= 2;
			x = 2 * x + 1;
		}
		else
		{
			x = 2 * x + 1;
			decode_number /= 2;
			degree++;
			break;
		}
		right *= 2; if (right > 1) right -= 1;
		left *= 2; if (left > 1) left -= 1;
		degree++;
		//printf("degree = %d\n", degree+1);
	}

	decode_number *= x;
	printf("%.30e <= \n%.30e < \n%.30e\n", new_left, decode_number, new_right);
}

void coding_text(char* msg)
{
	int str_len = strlen(msg);
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