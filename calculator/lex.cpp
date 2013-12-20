#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "lex.h"

static const char *cur_line;
static const char *p;

static int char_2_token_kind[256] = {0};

void lex_init() {
	const char *operators = "+-*/()";
	const int tokens[] = {ADD,SUB,MUL,DIV,LP,RP};
	for(int i = 0; i < sizeof(tokens); i++) {
		char_2_token_kind[operators[i]] = tokens[i];
	}
}

void set_cur_line(const char *line) {
	cur_line = line;
	p = cur_line;
}

static bool isnumber(char c) {
	return c >= '0' && c <= '9';
}

bool get_next_token(Token *tk) {
	bool number_scanning = false;
	double number_val = 0;
	int decimal_digits = 0;
	tk->_kind = NULTOKEN;
	while (true) {
		if (isnumber(*p)) {
			if (!number_scanning) {
				if (*p == '0' && (*(p+1) != '.' && isnumber(*(p+1)))) {
					fprintf(stderr, "lex failed : number can not start with 0 except for 0.x or 0\n");
					p++;
					return false;
				}
				number_val = *p - '0';
				number_scanning = true;
			}else {
				if (decimal_digits == 0) {
					number_val = number_val * 10 + *p - '0';
				}else if (decimal_digits > 0) {
					number_val = number_val + (*p - '0')/(pow(10.0,decimal_digits));
					decimal_digits++;
				}
			}
		}else if (*p == '.') {
			if (!number_scanning) {
				fprintf(stderr, "lex failed : number can not start with .\n");
				p++;
				return false;
			}
			if (decimal_digits != 0) {
				fprintf(stderr, "lex failed : number can not has multi-points\n");
				p++;
				return false;
			}
			decimal_digits = 1;
		}else {
			if (number_scanning) {
				if (decimal_digits == 1) {
					fprintf(stderr, "lex failed : number token can not end with .\n");
					return false;
				}
				decimal_digits = 0;
				number_scanning = false;
				tk->_kind = NUMBER;
				tk->_val = number_val;
				break;
			}
			if (!*p) break;
			else if(!isspace(*p)){
				int tkk = char_2_token_kind[*p];
				p++;				
				if (!tkk) {
					fprintf(stderr, "lex failed : unrecognized character %c\n", *p);
					return false;
				}
				tk->_kind = (TokenKind)tkk;
				break;
			}
		}
		p++;
	}
	return true;
}