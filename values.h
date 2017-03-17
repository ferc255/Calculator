#ifndef _VALUES_H_
#define _VALUES_H_

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

#define YYSTYPE int

#endif /* _VALUES_H_ */
