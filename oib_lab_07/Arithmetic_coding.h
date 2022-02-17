#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "config.h"
#define DEBUG

void find_interval(char* string, double* x, int* degree);
int save_interval(char* output_filename, double x, int degree);
void coding_text(char* input_filename, char* output_filename);
void decoding_text(char* input_filename, char* output_filename);