// Лабораторная работа №7
// Вариант: A + LZW 

#define _CRT_SECURE_NO_WARNINGS

#include "config.h"
#include "Arithmetic_coding.h"

int main(int argc, char* argv[])
{
    if(argc == 4)
    {
        if(argv[1][0] == CODE_SYMB)
        {
            coding_text(argv[2], argv[3]);
        }
        else if(argv[1][0] == DECODE_SYMB)
        {
            //decoding_text();
        }
        else
        {
            printf("Wrong parameters.\n"
                   "Input must have:\n"
                   "1) 'c' or 'd' for code or decode\n"
                   "2) input filename\n"
                   "3) output filename\n"
                   "Parametres: %s %s %s", argv[1], argv[2], argv[3]);
        }
    }
}