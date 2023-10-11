//Author: Geoffrey Dobson
//CS480 Lab2

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
	int boq;
};

//function declartions
void initStack(struct Stack *s);
void push(struct Stack *s,double value);
double pop(struct Stack *s);
void printStack(struct Stack *s);
void initQueue(struct Queue *q);
void enqueue(struct Queue *q,double value);
double dequeue(struct Queue *q);
void printQueue(struct Queue *q);

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
	printf("Queue test time \n");
	struct Queue q;
	initQueue(&q);
	printQueue(&q);
	enqueue(&q,3.14);
	enqueue(&q,9999);
	enqueue(&q,1337);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
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