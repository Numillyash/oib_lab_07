#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "config.h"

#define DEBUG
#define SYMB_IN_COUPLE 5

int filesize(FILE* fp);
void find_interval(char* string);
void coding_text(char* input_filename, char* output_filename);
void decoding_text(char* input_filename, char* output_filename);