#ifndef __LEX_H__
#define __LEX_H__

/*
Token : ( ) + - * / number \n \t
*/
enum TokenKind {LP=1, RP, ADD, SUB, MUL, DIV, NUMBER,NULTOKEN};

struct Token {
	TokenKind _kind;
	double _val;
};
void lex_init();
void set_cur_line(const char *line);

bool get_next_token(Token *tk);

#endif