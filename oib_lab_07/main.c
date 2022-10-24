// Лабораторная работа №7
// Вариант: LZW 

// LZW
// 
// 4
// Длительности 0.021 и 0.019
// Коеффицент 66.6666
// 
// 
//

#define _CRT_SECURE_NO_WARNINGS

#include "LZW.h"

int main(int argc, char* argv[])
{
    float start = 0, end = 0;
    if(argc == 4)
    {
            if (argv[1][0] == CODE_SYMB)
            {
                start = (float)clock();
                LZW_coding_text(argv[2], argv[3]);
                end = (float)clock();
                printf("Worktime : %.3f seconds\n", (end - start) / CLOCKS_PER_SEC);

            }
            else if (argv[1][0] == DECODE_SYMB)
            {
                start = (float)clock();
                LZW_decoding_text(argv[2], argv[3]);
                end = (float)clock();
                printf("Worktime : %.3f seconds\n", (end - start) / CLOCKS_PER_SEC);
            }
            else
            {
                printf("Wrong parameters.\n"
                    "Input must have:\n"
                    "1) 'c' or 'd' for code or decode\n"
                    "2) input filename\n"
                    "3) output filename\n"
                    "4) '1' or '2' for A coding or LZW coding\n"
                    "Parametres: %s %s %s", argv[1], argv[2], argv[3]);
            }     
    }
    else
        printf("Wrong parameters.\n"
            "Input must have:\n"
            "1) 'c' or 'd' for code or decode\n"
            "2) input filename\n"
            "3) output filename\n"
            "4) '1' or '2' for A coding or LZW coding\n");
}