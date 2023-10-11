#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//stack and queue needed for implementing shunting-yard algorithm
struct Stack
{
	double stack[100];
	int tos;
};

struct Queue
{
	struct Stack s1;
	struct Stack s2;
	int foq;
	int baq;
};

//function declartions
void push(struct Stack *s);
double pop(struct Stack *s);

int main()
{
	printf("Advanced Calc Starting!\n");
	return 0;
}