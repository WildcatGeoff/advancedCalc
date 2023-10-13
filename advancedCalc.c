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
double buildDecimal(struct Queue *q);
double getPrecedence(double in);
void evaluateExpression(struct Queue *out,struct Queue *index);

int main()
{
	char buffer[4096]; //buffer used for keyboard input
	bool calcRunning = true;
	struct Stack operatorStack;
	initStack(&operatorStack);
	struct Stack numberStack;//for tokenizing numbers
	initStack(&numberStack);
	struct Queue operatorIndexQueue;//since stacks and queues only hold doubles, used to keep track of operators in outputQueue
	initQueue(&operatorIndexQueue);
	struct Queue decimalQueue;//for tokenzing numbers with decimal points
	initQueue(&decimalQueue);
	struct Queue outputQueue;
	initQueue(&outputQueue);
	bool buildingANumber = false;
	bool buildingADecimal = false;
	double tempNum = 0;
	double tempDecimal = 0;
	double finalNum = 0;
	while(calcRunning)
	{
		printf("Calculator>");
		fgets(buffer,sizeof(buffer),stdin); //get input from keyboard
		char c;
		for(int i=0;i<4096;i++)
		{
			c = buffer[i];
			//48-57 ascii codes for numbers
			if(c >= 48 && c <= 57)
			{
				tempNum = c - 48;
				if(!buildingADecimal)
				{
					push(&numberStack,tempNum);
					buildingANumber = true;
				}
				else
				{
					enqueue(&decimalQueue,tempNum);
				}
			}
			else
			{
				if(c != 46)
				{
					if(buildingANumber)
					{
						finalNum = buildNum(&numberStack);
						if(!buildingADecimal)
						{
							enqueue(&outputQueue,finalNum);
							buildingANumber = false;
						}
					}
					if(buildingADecimal)
					{
						finalNum+=buildDecimal(&decimalQueue);
						enqueue(&outputQueue,finalNum);
						buildingANumber = false;
						buildingADecimal = false;
					}
				}
			}
			
			if(c == 46)// . character
			{
				if(!buildingADecimal)
				{
					buildingADecimal = true;
				}
				else
				{
					printf("ERROR: Too many decimal points in a row\n");
					break;
				}
			}
			
			if(c == 40 || c == 123)// ( or { characters
			{
				push(&operatorStack,c);
			}
			
			if(c == 41)// ) character
			{
				int opcode = 0;
				while(operatorStack.tos > 0)
				{
					opcode = (int)pop(&operatorStack);
					if(opcode != 40)
					{
						enqueue(&outputQueue,(double)opcode);
						enqueue(&operatorIndexQueue,outputQueue.s1.tos-1);
					}
					else
					{
						break;
					}
				}
			}
			
			if(c == 125)// } character
			{
				int opcode = 0;
				while(operatorStack.tos > 0)
				{
					opcode = (int)pop(&operatorStack);
					if(opcode != 123)
					{
						enqueue(&outputQueue,(double)opcode);
						enqueue(&operatorIndexQueue,outputQueue.s1.tos-1);
					}
					else
					{
						break;
					}
				}
			}
			
			if((c == 42) || (c == 43) || (c == 45) || (c == 47) || (c == 94))//*+-/^ ascii codes
			{
				double op;
				double p1;
				double p2;
				while(operatorStack.tos > 0)
				{
					op = pop(&operatorStack);
					p1 = getPrecedence(c);
					p2 = getPrecedence(op);
					if((int)op != 40 && (int)op != 123)
					{
						if(p2 < p1)
						{
							enqueue(&outputQueue,op);
							enqueue(&operatorIndexQueue,outputQueue.s1.tos-1);
						}
						else
						{
							push(&operatorStack,op);
							break;
						}
					}
					else
					{
						push(&operatorStack,op);
						break;
					}
				}
				push(&operatorStack,c);
			}
			
			if(c == 0)
			{
				//pop all operators still on the operator stack onto the output queue
				while(operatorStack.tos > 0)
				{
					enqueue(&outputQueue,pop(&operatorStack));
					enqueue(&operatorIndexQueue,outputQueue.s1.tos-1);
				}
				evaluateExpression(&outputQueue,&operatorIndexQueue);
				break;
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
double buildDecimal(struct Queue *q)
{
	double value = 0;
	double temp = 0;
	int power = 1;
	if(q->s1.tos > 0)
	{
		while(q->s1.tos > 0)
		{
			temp = pop(&(q->s1));
			value = value + (pow(10,-power)*temp);
			power++;
		}
	}
	else
	{
		value = pop(&(q->s1))*0.1;
	}
	return value;
}
double getPrecedence(double in)
{
	double out = 0;
	switch((int)in)
	{
		case 40:
			out = 1;
			break;
		case 42:
			out = 3;
			break;
		case 43:
			out = 4;
			break;
		case 45:
			out = 4;
			break;
		case 47:
			out = 3;
			break;
		case 94:
			out = 2;
			break;
	}
	return out;
}
void evaluateExpression(struct Queue *out,struct Queue *index)
{
	printQueue(out);
	int i = -1;
	int qIndex = 0;
	double answer = 0;
	double temp = 0;
	bool done = false;
	bool error = false;
	struct Stack eval;
	initStack(&eval);
	double a = 0;
	double b = 0;
	double x = 0;
	char *errorMsg;
	char divideByZero[] = "ERROR: Division by Zero\n";
	
	if(index->s1.tos > 0)
	{
		i = (int)dequeue(index);
	}
	while(!done)
	{
		if(out->s1.tos <= 0)
		{
			done = true;
		}
		else
		{
			temp = dequeue(out);
			if(i != qIndex)//its a number not an operator
			{
				push(&eval,temp);
			}
			else//it is an operator
			{
				switch((int)(temp))
				{
					case 42://*
						b = pop(&eval);
						a = pop(&eval);
						x = a*b;
						push(&eval,x);
						break;
					case 43://+
						b = pop(&eval);
						a = pop(&eval);
						x = a+b;
						push(&eval,x);
						break;
					case 47://division
						b = pop(&eval);
						a = pop(&eval);
						if((int)b != 0)
						{
							x = a/b;
							push(&eval,x);
						}
						else
						{
							error = true;
							done = true;
							errorMsg = divideByZero;
						}
						break;
				}
				if(index->s1.tos > 0)
				{
					i = (int)dequeue(index);
				}
			}
			qIndex++;
		}
	}
	if(!error)
	{
		if(eval.tos > 0)
		{
			answer = pop(&eval);
		}
		printf("Answer:> %f\n",answer);
	}
	else
	{
		printf("%s",errorMsg);
	}
}