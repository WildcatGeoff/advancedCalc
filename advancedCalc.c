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
void initStack(struct Stack *s);
void push(struct Stack *s,double value);
double pop(struct Stack *s);
void printStack(struct Stack *s);

int main()
{
	printf("Advanced Calc Starting!\n");
	struct Stack s;
	initStack(&s);
	printStack(&s);
	push(&s,33);
	push(&s,123);
	printStack(&s);
	printf("Popped %f \n",pop(&s));
	printStack(&s);
	return 0;
}

void initStack(struct Stack *s)
{
	s->tos = 0;
}
void push(struct Stack *s,double value)
{
	s->stack[s->tos] = value;
	s->tos++;
}
double pop(struct Stack *s)
{
	s->tos--;
	double num = s->stack[s->tos];
	return num;
}
void printStack(struct Stack *s)
{
	for(int i=s->tos-1;i>=0;i--)
	{
		if(i != 0)
		{
			printf("%f, ",s->stack[i]);
		}
		else
		{
			printf("%f\n",s->stack[i]);
		}
	}
	
	if(s->tos <= 0)
	{
		printf("Stack is empty\n");
	}
}