#include "Arithmetic_coding.h"
typedef struct
{
    short count;
    char symbol;
    double left_border;
    double right_border;
    double size;
} Ar_cod_char;

Ar_cod_char char_struct_array[ALPHABET_SIZE];

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
        char_struct_array[i].symbol = (char)(i+(int)'a');
        char_struct_array[i].count = 0;
        char_struct_array[i].size = 0;
        char_struct_array[i].left_border = 0;
        char_struct_array[i].right_border = 0;
	}

	for (i = 0; i < str_len; i++)
	{
		char_struct_array[(int)string[i] - (int)'a'].count++;
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
		ind = (int)string[i] - (int)'a';
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

int save_interval(char* output_filename)
{
    int i; // iterator
    int str_len = 0;
    FILE * FFp;
    FFp = fopen(output_filename, "a");

    if (FFp == NULL)
    {
        printf("Incorrect input filename\n");
        return 0;
    }
    for (i = 0; i < ALPHABET_SIZE; ++i) {
        if(char_struct_array[i].count)
        {
            str_len++;
        }
    }
    fprintf(FFp,"%d\n", str_len);
    for (i = 0; i < ALPHABET_SIZE; ++i) {
        if(char_struct_array[i].count)
        fprintf(FFp,"%c %f %f\n", char_struct_array[i].symbol,char_struct_array[i].left_border, char_struct_array[i].right_border);
    }
    fprintf(FFp,"%f\n", decode_number);
    fclose(FFp);
    return 1;
}

void coding_text(char* input_filename, char* output_filename)
{
    FILE * FFp, * FFp2;
    char* buff[11];
    int str_len;
    char* message;
    int c = 0;
    long long file_length = 0;
    char st[2];
    long long size = 0;

    FFp = fopen(input_filename, "r");
    FFp2 = fopen(output_filename, "w");
    if (FFp == NULL || FFp2 == NULL)
    {
        printf("Incorrect input filename\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fseek (FFp, 0, SEEK_END);
        size = ftell(FFp);
        fclose(FFp);
        FFp = fopen(input_filename, "r");
        size = (size%10 == 0)? size/10 : (size/10)+1;

        fprintf(FFp2,"Start of file. Number of couples: %llu\n", size);
        fclose(FFp2);
        str_len = 10;
        message = (char*)malloc(str_len + 2);
        message[0] = '\0';

        while (c != EOF) {
            c = fgetc(FFp);
            if(c == EOF)
            {
                if((int)strlen(message) != 0){
                    strcat(message, "{");
                    find_interval(message);
                    if(save_interval(output_filename) == 0)
                    {
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                st[0] = c; st[1] = '\0';
                strcat(message, st);
                file_length++;
                if(file_length % 10 == 0)
                {
                    strcat(message, "{");
                    find_interval(message);
                    if(save_interval(output_filename) == 0)
                    {
                        fclose(FFp);
                        exit(EXIT_FAILURE);
                    }
                    message[0] = '\0';
                }

            }
        }
        FFp2 = fopen(output_filename, "a");
        fprintf(FFp2,"End of file");
    }
    fclose(FFp);
}

void decoding_text(char* input_filename, char* output_filename)
{
	double curr_left, curr_size, new_left = 0, new_size = 0, new_right;
	double prev_left = 0, prev_size = 1;
	char symb = ' ';


	printf("Text is: \n");
	while (symb != '{')
	{
		for (int i = 0; i < 27; i++)
		{
			if (char_struct_array[i].count)
			{
				curr_left = char_struct_array[i].left_border;
				curr_size = char_struct_array[i].size;
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