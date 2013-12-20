#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"

#define MAX_BUF_LEN 2048

int main(int argc, char *argv[]) {
	char line[MAX_BUF_LEN];
	lex_init();
	parse_init();
	while (fgets(line, MAX_BUF_LEN, stdin)) {
		double val;
		if (parse_line(line, &val))
			printf("%f\n", val);
		else
			printf("error\n");
	}
	return 0;
}