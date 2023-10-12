//Author: Geoffrey Dobson
//CS480 Lab2

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
	int boq;
};

//function declartions
void initStack(struct Stack *s);
void push(struct Stack *s,double value);
double pop(struct Stack *s);
void printStack(struct Stack *s);
double buildNum(struct Stack *s);
void initQueue(struct Queue *q);
void enqueue(struct Queue *q,double value);
double dequeue(struct Queue *q);
void printQueue(struct Queue *q);

int main()
{
	char buffer[4096]; //buffer used for keyboard input
	bool calcRunning = true;
	struct Stack operatorStack;
	initStack(&operatorStack);
	struct Stack numberStack;//for tokenizing numbers
	initStack(&numberStack);
	struct Queue outputQueue;
	initQueue(&outputQueue);
	bool buildingANumber = false;
	double tempNum = 0;
	while(calcRunning)
	{
		printf("Calculator>");
		fgets(buffer,sizeof(buffer),stdin); //get input from keyboard
		char c;
		for(int i=0;i<4096;i++)
		{
			c = buffer[i];
			if(c == 0)
			{
				if(buildingANumber)
				{
					tempNum = buildNum(&numberStack);
					printf("Number built: %f\n",tempNum);
					buildingANumber = false;
				}
				break;
			}
			//48-57 ascii codes for numbers
			if(c >= 48 && c <= 57)
			{
				tempNum = c - 48;
				push(&numberStack,tempNum);
				buildingANumber = true;
			}
			else
			{
				if(buildingANumber)
				{
					tempNum = buildNum(&numberStack);
					printf("Number built: %f\n",tempNum);
					buildingANumber = false;
				}
			}
		}
		calcRunning = false;
	}
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
	if(s->tos > 0)
	{
		s->tos--;
	}
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
//idea behind this function is that when the main program finds ascii number characters
//it will push them onto a special stack
//once all the characters are found, it will convert the characters into a double
//by popping them off the stack and multiplying by the power of 10 that it is
double buildNum(struct Stack *s)
{
	double value = 0;
	double temp = 0;
	int power = 0;
	if(s->tos > 0)
	{
		while(s->tos > 0)
		{
			temp = pop(s);
			value = value + (pow(10,power)*temp);
			power++;
		}
	}
	else
	{
		value = s->stack[0];
	}
	return value;
}
void initQueue(struct Queue *q)
{
	initStack(&(q->s1));
	initStack(&(q->s2));
	q->boq = 0;
}
void enqueue(struct Queue *q,double value)
{
	if(q->s1.tos > 0)
	{
		while(q->s1.tos > 0)
		{
			push(&(q->s2),pop(&(q->s1)));
		}
		push(&(q->s1),value);
		while(q->s2.tos > 0)
		{
			push(&(q->s1),pop(&(q->s2)));
		}
	}
	else
	{
		push(&(q->s1),value);
	}
}
double dequeue(struct Queue *q)
{
	double value = pop(&(q->s1));
	return value;
}
void printQueue(struct Queue *q)
{
	printStack(&(q->s1));
}