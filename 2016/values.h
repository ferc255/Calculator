#ifndef _VALUES_
#define _VALUES_

typedef enum 
{ 
	VT_INT, 
	VT_FLOAT,
	VT_COMPLEX
} value_type_t;

typedef long double complex complex_t;

typedef struct
{
	value_type_t type;
	union
	{
		long long llint;
		long double ldl;
		complex_t xldl;
	};
} value_t;

#endif
