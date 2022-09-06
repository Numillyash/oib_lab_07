#include "Arithmetic_coding.h"
typedef struct
{
	short count;
	unsigned char symbol;
	double left_border;
	double right_border;
	double size;
} Ar_cod_char;

Ar_cod_char char_struct_array[ALPHABET_SIZE];

double* x; int* degree; // for double x / (2^degree) number
double decode_number = 1, degree_minus;

int filesize(FILE* fp)
{
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	rewind(fp);
	return sz;
}

void find_interval(unsigned char* string)
{
	*x = 0; *degree = 0;
	int str_len = SYMB_IN_COUPLE;
	int i = 0; // iterator
	double curr_left, curr_size, new_left = 0, new_size = 0, new_right = 0;
	double prev_left = 0, prev_size = 1;
	int ind; // index of current symbol
	double right, left; // interval borders that will be changed due sicle
	

	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		char_struct_array[i].symbol = (char)(i);
		char_struct_array[i].count = 0;
		char_struct_array[i].size = 0;
		char_struct_array[i].left_border = 0;
		char_struct_array[i].right_border = 0;
	}

	for (i = 0; i < str_len; i++)
	{
		char_struct_array[(int)string[i]].count++;
	}

	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		if (char_struct_array[i].count)
		{
			char_struct_array[i].size = (double)char_struct_array[i].count / str_len;
			if (i)
			{
				char_struct_array[i].left_border = new_left + new_size;
			}
			else
			{
				char_struct_array[i].left_border = 0;
			}
			new_left = char_struct_array[i].left_border;
			new_size = char_struct_array[i].size;
			char_struct_array[i].right_border = new_left + new_size;
		}
	}

	for (int i = 0; i < str_len; i++)
	{
		ind = (int)string[i];
		curr_left = char_struct_array[ind].left_border;
		curr_size = char_struct_array[ind].size;
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
			*x = 2 * *x + 0;
		}
		else if (0.5 < left)
		{
			decode_number /= 2;
			*x = 2 * *x + 1;
		}
		else if (0.5 == right)
		{
			degree_minus = 0.5;
			*x = 2 * *x + 0;
			(*degree)++;
			decode_number /= 2;
			while (right - degree_minus < left) {
				degree_minus /= 2;
				*x = 2 * *x + 1;
				(*degree)++;
				decode_number /= 2;
			}
			break;
		}
		else
		{
			*x = 2 * *x + 1;
			decode_number /= 2;
			(*degree)++;
			break;
		}
		right *= 2; if (right >= 1) right -= 1;
		left *= 2; if (left >= 1) left -= 1;
		if (right == 0 && left == 0)
			break;
		(*degree)++;

	}

	decode_number *= *x;
}

int save_interval(char* output_filename, FILE* FFp)
{
	int i; // iterator
	int str_len = 0;

	if (FFp == NULL)
	{
		printf("Incorrect input filename\n");
		return 0;
	}
	for (i = 0; i < ALPHABET_SIZE; ++i) {
		if (char_struct_array[i].count)
		{
			str_len++;
		}
	}
	fprintf(FFp, "%d\n", str_len);
	for (i = 0; i < ALPHABET_SIZE; ++i) {
		if (char_struct_array[i].count)
			fprintf(FFp, "%c %lf %lf\n", char_struct_array[i].symbol, char_struct_array[i].left_border, char_struct_array[i].right_border);
	}
	fprintf(FFp, "%lf\n", decode_number);
	decode_number = 1;
	*degree = 0;
	*x = 0;
	return 1;
}

void coding_text(char* input_filename, char* output_filename)
{
	double idk; x = &idk;
	int idk2; degree = &idk2;
	FILE* FFp, * FFp2;
	int str_len;
	unsigned char message[SYMB_IN_COUPLE+2];
	unsigned int  c = 0;
	long long file_length = 0;
	char st[2];
	long long size = 0;

	long long size_real = 0;

	FFp = fopen(input_filename, "rb");
	FFp2 = fopen(output_filename, "wb");
	if (FFp == NULL || FFp2 == NULL)
	{
		printf("Incorrect input filename\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		fseek(FFp, 0, SEEK_END);
		size = ftell(FFp);
		fclose(FFp);
		FFp = fopen(input_filename, "rb");
		size = (size % SYMB_IN_COUPLE == 0) ? size / SYMB_IN_COUPLE : (size / SYMB_IN_COUPLE) + 1;

		fprintf(FFp2, "Start of file. Number of couples: %llu\n", size);
		//fclose(FFp2);
		str_len = SYMB_IN_COUPLE;
		for (size_real = 0; size_real < SYMB_IN_COUPLE + 2; size_real++)
			message[size_real] = '\0';

		while ((c = getc(FFp)) != (unsigned)EOF)
		{
			message[file_length] = c;
			message[file_length+1] = '\0';
			file_length++;
			if (file_length == SYMB_IN_COUPLE)
			{
				//
				//printf("|Message in this couple: |%s|\n", message);
				find_interval(message);
				if (save_interval(output_filename, FFp2) == 0)
				{
					exit(EXIT_FAILURE);
				}
				file_length = 0;
				message[0] = '\0';
			}
		}
		if (file_length != 0)
		{
			find_interval(message);
			if (save_interval(output_filename, FFp2) == 0)
			{
				exit(EXIT_FAILURE);
			}
			file_length = 0;
			message[0] = '\0';
		}
		//FFp2 = fopen(output_filename, "a");
		fprintf(FFp2, "End of file");
	}
	fclose(FFp);
	fclose(FFp2);

	FFp = fopen(input_filename, "rb");
	FFp2 = fopen(output_filename, "rb");
	int into = filesize(FFp);
	int outof = filesize(FFp2);
	float koef = (float)into / (float)outof;
	printf("Input file size : %d byte\nOutput file size : %d byte\nCompression : %f\n", into, outof, koef);
	fclose(FFp);
	fclose(FFp2);

}

void read_couple(FILE* input, FILE* output)
{
	char symb = ' ';
	double new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;
	int i, j, char_in_couple;

	for (i = 0; i < ALPHABET_SIZE; i++)
	{
		char_struct_array[i].symbol = (unsigned char)(i);
		char_struct_array[i].count = 0;
		char_struct_array[i].size = 0;
		char_struct_array[i].left_border = 0;
		char_struct_array[i].right_border = 0;
	}

	fscanf(input, "%d\n", &char_in_couple);

	for (i = 0; i < char_in_couple; i++)
	{
		fscanf(input, "%c %lf%lf\n", &symb, &new_left, &new_right);
		char_struct_array[(unsigned char)symb].left_border = new_left;
		char_struct_array[(unsigned char)symb].right_border = new_right;
		char_struct_array[(unsigned char)symb].size = new_right - new_left;
		char_struct_array[(unsigned char)symb].count = 1;
	}

	fscanf(input, "%lf\n", &decode_number);
	/*for (i = 0; i < ALPHABET_SIZE; i++)
	{
		if (char_struct_array[i].count)
		{
			printf("%c %f %f\n", char_struct_array[i].symbol, char_struct_array[i].left_border, char_struct_array[i].right_border);
		}
	}*/

	for (j = 0; j < SYMB_IN_COUPLE; j++)
	{
		for (i = 0; i < ALPHABET_SIZE; i++)
		{
			if (char_struct_array[i].count && char_struct_array[i].left_border <= decode_number && char_struct_array[i].right_border > decode_number)
			{
				symb = char_struct_array[i].symbol;
					fprintf(output, "%c", char_struct_array[i].symbol);
					//printf("%c", char_struct_array[i].symbol);
				/*printf("%c\n\n", char_struct_array[i].symbol);*/

				new_left = char_struct_array[i].left_border;
				new_size = char_struct_array[i].size;
				break;
			}
		}


		new_right = new_left;

		for (i = 0; i < ALPHABET_SIZE; i++)
		{
			if (char_struct_array[i].count)
			{
				char_struct_array[i].left_border = new_right;
				char_struct_array[i].size *= new_size / prev_size;
				new_right = char_struct_array[i].left_border + char_struct_array[i].size;
				char_struct_array[i].right_border = new_right;
			}
		}

		prev_size = new_size;
		prev_left = new_left;
	}
}

void decoding_text(char* input_filename, char* output_filename)
{
	int couple, i;
	FILE* FFp, * FFp2;

	FFp = fopen(input_filename, "rb");
	FFp2 = fopen(output_filename, "wb");

	if (FFp == NULL || FFp2 == NULL)
	{
		printf("Incorrect input filename\n");
		exit(EXIT_FAILURE);
	}

	fscanf(FFp, "Start of file. Number of couples: %d\n", &couple);
	for (i = 0; i < couple; i++)
	{
		read_couple(FFp, FFp2);
		//printf("Done %d couple\n", i);
	}
	fclose(FFp); fclose(FFp2);
}