#ifndef _VALUES_
#define _VALUES_

typedef enum
{
	NUMBER,
	PLUS,
	MUL,
	LPAREN,
	RPAREN,
	END,
	MINUS,
	DIV,
} token_t;

typedef struct
{
	token_t token;
	int number;
} state_t;

static int c2t[256] = 
{
	[0 ... 255] = END,
	['+'] = PLUS,
	['*'] = MUL,
	['('] = LPAREN,
	[')'] = RPAREN,
	['-'] = MINUS,
	['/'] = DIV,
};

#endif 
