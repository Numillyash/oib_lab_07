#include "LZW.h"

#define BITS 12
#define HASHING_SHIFT BITS - 8
#define MAX_VALUE (1 << BITS) - 1
#define MAX_CODE MAX_VALUE - 1
#define TABLE_SIZE 20000

int code_value[TABLE_SIZE];
unsigned int prefix_code[TABLE_SIZE];
unsigned char append_character[TABLE_SIZE];
unsigned char decode_stack[4000];

output_code(FILE* output, unsigned int code)
{
    static int output_bit_count = 0;
    static unsigned long output_bit_buffer = 0L;
    output_bit_buffer |= (unsigned long)code << (32 - BITS - output_bit_count);
    output_bit_count += BITS;
    while (output_bit_count >= 8)
    {
        putc(output_bit_buffer >> 24, output);
        output_bit_buffer <<= 8;
        output_bit_count -= 8;
    }
}

find_match(int hash_prefix, unsigned int hash_character)
{
    int index;
    int offset;

    index = (hash_character << (HASHING_SHIFT)) ^ hash_prefix;
    if (index == 0)
        offset = 1;
    else
        offset = TABLE_SIZE - index;
    while (1)
    {
        if (code_value[index] == -1)
            return (index);
        if (prefix_code[index] == hash_prefix && append_character[index] == hash_character)
            return (index);
        index -= offset;
        if (index < 0)
            index += TABLE_SIZE;
    }
}

encode(FILE* input, FILE* output)
{
    unsigned int next_code;
    unsigned int character;
    unsigned int string_code;
    unsigned int index;
    int i;
    next_code = 256;
    for (i = 0; i < TABLE_SIZE; i++)
        code_value[i] = -1;
    i = 0;
    string_code = getc(input);
    while ((character = getc(input)) != (unsigned)EOF)
    {
        if (++i == 1000)
            i = 0;

        index = find_match(string_code, character);
        if (code_value[index] != -1)
            string_code = code_value[index];
        else
        {
            if (next_code <= MAX_CODE)
            {
                code_value[index] = next_code++;
                prefix_code[index] = string_code;
                append_character[index] = character;
            }
            output_code(output, string_code);
            string_code = character;
        }
    }

    output_code(output, string_code);
    output_code(output, MAX_VALUE);
    output_code(output, 0);
}

input_code(FILE* input)
{
    unsigned int return_value;
    static int input_bit_count = 0;
    static unsigned long input_bit_buffer = 0L;
    while (input_bit_count <= 24)
    {
        input_bit_buffer |= (unsigned long)getc(input) << (24 - input_bit_count);
        input_bit_count += 8;
    }
    return_value = input_bit_buffer >> (32 - BITS);
    input_bit_buffer <<= BITS;
    input_bit_count -= BITS;
    return (return_value);
}

char* decode_string(unsigned char* buffer, unsigned int code)
{
    int i;

    i = 0;
    while (code > 255)
    {
        *buffer++ = append_character[code];
        code = prefix_code[code];
        if (i++ >= 4094)
        {
            printf("Fatal error during code expansion.\n");
            exit(0);
        }
    }
    *buffer = code;
    return (buffer);
}

decode(FILE* input, FILE* output)
{
    unsigned int next_code;
    unsigned int new_code;
    unsigned int old_code;
    int character;
    unsigned char* string;
    next_code = 256;

    old_code = input_code(input);
    character = old_code;
    putc(old_code, output);

    while ((new_code = input_code(input)) != (MAX_VALUE))
    {
        if (new_code >= next_code)
        {
            *decode_stack = character;
            string = decode_string(decode_stack + 1, old_code);
        }

        else
            string = decode_string(decode_stack, new_code);

        character = *string;
        while (string >= decode_stack)
            putc(*string--, output);

        if (next_code <= MAX_CODE)
        {
            prefix_code[next_code] = old_code;
            append_character[next_code] = character;
            next_code++;
        }
        old_code = new_code;
    }
}

void LZW_coding_text(char* input_filename, char* output_filename)
{
    FILE* FFp, * FFp2;

    FFp = fopen(input_filename, "rb");
    FFp2 = fopen(output_filename, "wb");
    if (FFp == NULL || FFp2 == NULL)
    {
        printf("Incorrect input filename\n");
        exit(EXIT_FAILURE);
    }

    encode(FFp, FFp2);
    fclose(FFp);
    fclose(FFp2);

    FFp = fopen(input_filename, "r");
    FFp2 = fopen(output_filename, "r");
    int into = filesize(FFp);
    int outof = filesize(FFp2);
    float koef = (float)into / (float)outof;
    printf("Input file size : %d byte\nOutput file size : %d byte\nCompression : %f\n", into, outof, koef);
    fclose(FFp);
    fclose(FFp2);
}

void LZW_decoding_text(char* input_filename, char* output_filename)
{
    FILE *FFp, *FFp2;

    FFp = fopen(input_filename, "rb");
    FFp2 = fopen(output_filename, "wb");
    if (FFp == NULL || FFp2 == NULL)
    {
        printf("Incorrect input filename\n");
        exit(EXIT_FAILURE);
    }

    decode(FFp, FFp2);

    fclose(FFp);
    fclose(FFp2);
}