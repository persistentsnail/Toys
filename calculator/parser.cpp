#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "lex.h"
#include "parser.h"


/*
line = expression;
expression = term | term ADD expression | term SUB expression;
term = primary_expression | primary_expression MUL term | primary_expression DIV term
primary_expression = NUMBER | SUB primary_expression | LP expression RP
*/

#define CALL_FAILED_RET(expr)  \
	if (!expr) \
		return false; \


static Token cur_token;
static bool token_has_got = false;

static bool get_token(Token *tk) {
	bool ret = true;
	if (!token_has_got) {
		ret = get_next_token(&cur_token);
	} else
		token_has_got = false;
	*tk = cur_token;
	return ret;
}

static void unget_token() {
	assert(token_has_got == false);
	token_has_got = true;
}

static bool parse_expression(double *value);

static bool parse_primary_expression(double *value) {
	Token tk;
	CALL_FAILED_RET(get_token(&tk));
	if (tk._kind == NUMBER) {
		*value = tk._val;
		return true;
	}else if (tk._kind == SUB) {
		double tmp;
		CALL_FAILED_RET(parse_primary_expression(&tmp));
		*value = -(tmp);
	}else if (tk._kind == LP) {
		CALL_FAILED_RET(parse_expression(value));
		CALL_FAILED_RET(get_token(&tk));
		if (tk._kind != RP) return false; 
	}else
		return false;
	return true;
}

static bool parse_term(double *value) {
	CALL_FAILED_RET(parse_primary_expression(value));
	for (;;) {
		Token tk;
		CALL_FAILED_RET(get_token(&tk));
		if (tk._kind == MUL || tk._kind == DIV) {
			double tmp;
			CALL_FAILED_RET(parse_primary_expression(&tmp));
			*value = tk._kind == MUL? *value * tmp : *value / tmp;
		}else {
			unget_token();
			break;
		}
	}
	return true;
}

static bool parse_expression(double *value) {
	CALL_FAILED_RET(parse_term(value));
	for (;;) {
		Token tk;
		CALL_FAILED_RET(get_token(&tk));
		if (tk._kind == ADD || tk._kind == SUB) {
			double tmp;
			CALL_FAILED_RET(parse_term(&tmp));
			*value = tk._kind == ADD? *value + tmp : *value-tmp;
		}else {
			unget_token();
			break;
		}
	}
	return true;
}

bool parse_line(const char *line, double *value) {
	set_cur_line(line);
	token_has_got = false;
	if (parse_expression(value)) {
		Token tk;
		CALL_FAILED_RET(get_token(&tk));
		if (tk._kind != NULTOKEN) return false;
	}
}

void parse_init() {

}