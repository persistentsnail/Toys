#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"

const char* token_2_str[] = {"","LP", "RP", "ADD", "SUB", "MUL", "DIV", "NUMBER",};

	
void LEX_TEST(const char *line) {
	printf("%s token is :\n", line);
	set_cur_line(line);
	while(true) {
		Token tk;		
		if (!get_next_token(&tk) || tk._kind == NULTOKEN) {		
			break;	
		}		
		printf("%s ",token_2_str[tk._kind]);		
		if (tk._kind == NUMBER) printf("%f", tk._val);		
		printf("\n");		
	}	
}

void PARSER_TEST(const char *line) {
	printf("line %s = \n", line);	
	double result;					
	if(parse_line(line, &result)) {
		printf("%f\n", result);	
	}else {				
		printf("parse error!!!\n");	
	}
}

void test_run() {
	
	lex_init();
	LEX_TEST("3 + 4 * 5");
	LEX_TEST("3*(4.567 +    1204.01/9) *3-50.67 + 0.001");
	
	lex_init();
	parse_init();
	PARSER_TEST("3 + 4 * 5");
	PARSER_TEST("3*(4.567 +    1204.01/9) *3-50.67 + 0.001");
	return;
}
