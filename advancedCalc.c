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
	double stack[300];
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
int sanityCheck(char buf[]);
void evaluateExpression(struct Queue *out,struct Queue *index);

int main()
{
	char buffer[4096]; //buffer used for keyboard input
	bool calcRunning = true;
	struct Stack operatorStack;
	struct Stack numberStack;//for tokenizing numbers
	struct Queue operatorIndexQueue;//since stacks and queues only hold doubles, used to keep track of operators in outputQueue
	struct Queue decimalQueue;//for tokenzing numbers with decimal points
	struct Queue outputQueue;//queue that eventually will hold numbers and operators in polish notation
	bool buildingANumber = false;
	bool buildingADecimal = false;//bools for number building functions
	double tempNum = 0;
	double tempDecimal = 0;
	double finalNum = 0;
	bool isUnary = false;
	
	printf("Starting advancedCalc, enter a q to quit\n");
	
	while(calcRunning)
	{
		initStack(&operatorStack);
		initStack(&numberStack);
		initQueue(&operatorIndexQueue);//reset state of stacks and queues whenever looping
		initQueue(&decimalQueue);
		initQueue(&outputQueue);
		printf("Calculator>");
		fgets(buffer,sizeof(buffer),stdin); //get input from keyboard
		char c;
		for(int i=0;i<4096;i++)
		{
			c = buffer[i];
			//48-57 ascii codes for numbers
			if(c >= 48 && c <= 57)
			{
				tempNum = c - 48;//convert from ascii to 0-9 value
				if(!buildingADecimal)
				{
					push(&numberStack,tempNum);//push onto normal number stack
					buildingANumber = true;
				}
				else
				{
					enqueue(&decimalQueue,tempNum);//its a number after the decimal point, put it on decimal queue
				}
			}
			else
			{
				if(c != 46)// .character
				{
					if(buildingANumber)
					{
						finalNum = buildNum(&numberStack);//entered in a character thats not a number or . so completely build number from characters
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
					printf("ERROR: Too many decimal points in a row\n");//gets thrown when a number with multiple decimal points is found
					break;
				}
			}
			
			if(c == 40 || c == 123)// ( or { characters
			{
				push(&operatorStack,c);//with the shunting-yard algorithm, push ( or { on stack
			}
			
			if(c == 41)// ) character
			{
				int opcode = 0;
				while(operatorStack.tos > 0)
				{
					opcode = (int)pop(&operatorStack);//look on operator stack for ( character
					if(opcode != 40)
					{
						enqueue(&outputQueue,(double)opcode);//enqueue everything thats not (
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
				opcode = (int)pop(&operatorStack);//same as ) but with }
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
			
			if(c == 45)// - is it unary or subtraction?
			{
				if(i!=0)
				{
					char uTest;
					for(int j=i-1;j>=0;j--)
					{
						uTest = buffer[j];
						if(uTest >= 48 && uTest <= 57)
						{
							isUnary = false;
							break;
						}
						else if(uTest == 32)//empty space
						{
						}
						else//not a number or empty space, must be some sort of operator or function and therefore is unary
						{
							isUnary = true;
							break;
						}
					}
				}
				else
				{
					isUnary = true;//first character, must be unary
				}
			}
			
			if((c == 42) || (c == 43) || (c == 45) || (c == 47) || (c == 94))//*+-/^ ascii codes
			{
				double op;
				double p1;
				double p2;
				if(c == 45 && isUnary == true)
				{
					c = 1;
					isUnary = false;
				}
				while(operatorStack.tos > 0)
				{
					op = pop(&operatorStack);//compare operator precedence, throw higher precedence operators on queue
					p1 = getPrecedence(c);
					p2 = getPrecedence(op);
					if((int)op != 40 && (int)op != 123)
					{
						if(p2 < p1)//lower number means higher precedence
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
			
			if(c == 113)//q, quits the calculator
			{
				calcRunning = false;
				break;
			}
			if(c >= 65 && c <= 90)//capital letters
			{
				c = c+32;//convert capital letters to lowercase
			}
			if(c >= 97 && c <= 122)//lowercase letters
			{
				char nextLetter = 0;//the next lines just check characters in front to form a function like cos and convert it to a double for the operator stack
				double functionType = 0;
				if(c == 99)//c
				{
					nextLetter = buffer[i+1];
					if(nextLetter != 111)
					{
						printf("ERROR: illegal character\n");
						break;
					}
					else
					{
						nextLetter = buffer[i+2];
						if(nextLetter == 115)
						{
							nextLetter = buffer[i+3];
							if(nextLetter != 40)
							{
								printf("ERROR: ( expected\n");
								break;
							}
							else
							{
								//cos function
								functionType = 2;
								push(&operatorStack,functionType);
								i = i+2;
							}
						}
						else if(nextLetter == 116)
						{
							nextLetter = buffer[i+3];
							if(nextLetter != 40)
							{
								printf("ERROR: ( expected\n");
								break;
							}
							else
							{
								//cot function
								functionType = 3;
								push(&operatorStack,functionType);
								i = i+2;
							}
						}
						else
						{
							printf("ERROR: illegal character\n");
							break;
						}
					}
				}
				else if(c == 108)//l
				{
					nextLetter = buffer[i+1];
					if(nextLetter == 110)
					{
						nextLetter = buffer[i+2];
						if(nextLetter != 40)
						{
							printf("ERROR: ( expected\n");
							break;
						}
						else
						{
							//ln function
							functionType = 4;
							push(&operatorStack,functionType);
							i = i+1;
						}
					}
					else if(nextLetter == 111)
					{
						nextLetter = buffer[i+2];
						if(nextLetter == 103)
						{
							nextLetter = buffer[i+3];
							if(nextLetter != 40)
							{
								printf("ERROR: ( expected\n");
								break;
							}
							else
							{
								//log function
								functionType = 5;
								push(&operatorStack,functionType);
								i = i+2;
							}
						}
						else
						{
							printf("ERROR: illegal character\n");
							break;
						}
					}
					else
					{
						printf("ERROR: illegal character\n");
						break;
					}
				}
				else if(c == 115)
				{
					nextLetter = buffer[i+1];
					if(nextLetter == 105)
					{
						nextLetter = buffer[i+2];
						if(nextLetter == 110)
						{
							nextLetter = buffer[i+3];
							if(nextLetter != 40)
							{
								printf("ERROR: ( expected\n");
								break;
							}
							else
							{
								//sine function
								functionType = 6;
								push(&operatorStack,functionType);
								i = i+2;
							}
						}
						else
						{
							printf("ERROR: illegal character\n");
							break;
						}
					}
					else
					{
						printf("ERROR: illegal character\n");
						break;
					}
				}
				else if(c == 116)
				{
					nextLetter = buffer[i+1];
					if(nextLetter == 97)
					{
						nextLetter = buffer[i+2];
						if(nextLetter == 110)
						{
							nextLetter = buffer[i+3];
							if(nextLetter != 40)
							{
								printf("ERROR: ( expected\n");
								break;
							}
							else
							{
								//tangent function
								functionType = 7;
								push(&operatorStack,functionType);
								i = i+2;
							}
						}
						else
						{
							printf("ERROR: illegal character\n");
							break;
						}
					}
					else
					{
						printf("ERROR: illegal character\n");
						break;
					}
				}
			}
			
			if(c == 0)//null terminator
			{
				//pop all operators still on the operator stack onto the output queue
				while(operatorStack.tos > 0)
				{
					enqueue(&outputQueue,pop(&operatorStack));
					enqueue(&operatorIndexQueue,outputQueue.s1.tos-1);
				}
				int sanity = sanityCheck(buffer);
				if(sanity == 0)
				{
					evaluateExpression(&outputQueue,&operatorIndexQueue);
				}
				else
				{
					switch(sanity)
					{
						case 1:
							printf("ERROR: Expression doesn't contain a number\n");
							break;
						case 2:
							printf("ERROR: Mismatched parenthesis or brackets\n");
							break;
						case 3:
							printf("ERROR: Operator missing operand\n");
							break;
						case 4:
							printf("ERROR: Operator missing operand\n");
							break;
					}
				}
				break;
			}
		}
	}
	return 0;
}

void initStack(struct Stack *s)
{
	s->tos = 0;//make sure top of stack is pointing to 0
}
void push(struct Stack *s,double value)
{
	s->stack[s->tos] = value;//basic push operation of stack
	s->tos++;
}
double pop(struct Stack *s)
{
	if(s->tos > 0)
	{
		s->tos--;
	}
	double num = s->stack[s->tos];//pop stack, move tos
	return num;
}
void printStack(struct Stack *s)//super useful function for debugging purposes
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
			if(power > 300)
			{
				printf("Error: Out of Range");
			}
		}
	}
	else
	{
		value = s->stack[0];
	}
	return value;
}
void initQueue(struct Queue *q)//queue is made of stacks, init the stacks
{
	initStack(&(q->s1));
	initStack(&(q->s2));
	q->boq = 0;
}
void enqueue(struct Queue *q,double value)//enqueue by popping one stack to another then pushing first element O(n)
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
double dequeue(struct Queue *q)//first element is on top of first stack O(1)
{
	double value = pop(&(q->s1));
	return value;
}
void printQueue(struct Queue *q)
{
	printStack(&(q->s1));
}
double buildDecimal(struct Queue *q)// like the build number function but for the right of the decimal
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
			if(power > 300)
			{
				printf("Error: Out of Range");
			}
		}
	}
	else
	{
		value = pop(&(q->s1))*0.1;
	}
	return value;
}
double getPrecedence(double in)//input is function num, output is precedence
{
	double out = 0;
	switch((int)in)
	{
		case 1:
			out = 2;
			break;
		case 2:
			out = 2;
			break;
		case 3:
			out = 2;
			break;
		case 4:
			out = 2;
			break;
		case 5:
			out = 2;
			break;
		case 6:
			out = 2;
			break;
		case 7:
			out = 2;
			break;
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
int sanityCheck(char buf[])// function that makes basic checks to see if input is formatted correctly
{
	int sanity = 0;
	char c;
	int leftP = 0;
	int rightP = 0;
	int leftB = 0;
	int rightB = 0;
	bool hasNumber = false;
	bool lastNotLeast = false;
	int endex = 0;
	int pBalance = 0;
	int bBalance = 0;
	bool balanceOff = false;
	bool badRight = false;
	bool badStart = false;
	bool notSpace = false;
	
	
	for(int i=0;i<4096;i++)
	{
		c = buf[i];
		if(!notSpace)
		{
			if(c == 42 || c == 43 || c == 47 || c == 94)
			{
				badStart = true;
				notSpace = true;
			}
			if(c == 32)
			{
			}
			else
			{
				notSpace = true;
			}
		}
		if(c >= 48 && c <= 57)
		{
			hasNumber = true;
		}
		if(c == 40)
		{
			leftP++;
			pBalance--;
		}
		if(c == 41)
		{
			rightP++;
			pBalance++;
			if(pBalance > 1)
			{
				balanceOff = true;
			}
		}
		if(c == 123)
		{
			leftB++;
			bBalance--;
		}
		if(c == 125)
		{
			rightB++;
			bBalance++;
			if(bBalance > 1)
			{
				balanceOff = true;
			}
		}
		if((c >=42 && c<=47) || c == 94)
		{
			char t = buf[i+1];
			if(t == 41)
			{
				badRight = true;
			}
		}
		if(c == 0)
		{
			endex = i;
			break;
		}
	}
	if(leftP != rightP)
	{
		balanceOff = true;
	}
	if(leftB != rightB)
	{
		balanceOff = true;
	}
	for(int j=endex-1;j>0;j--)
	{
		c = buf[j];
		if((c >=42 && c<=47) || c == 94 || c == 40)
		{
			lastNotLeast = true;
			break;
		}
		else if((c >= 48 && c <= 57)|| c == 41) 
		{
			lastNotLeast = false;
			break;
		}
	}
	if(!hasNumber)
	{
		sanity = 1;
	}
	else if(balanceOff)
	{
		sanity = 2;
	}
	else if(lastNotLeast)
	{
		sanity = 3;
	}
	else if(badRight)
	{
		sanity = 4;
	}
	else if(badStart)
	{
		sanity = 4;
	}
	return sanity;
}
void evaluateExpression(struct Queue *out,struct Queue *index)//function that does heavy lifting to convert polish notation to answer
{
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
	char undefinedFunction[] = "ERROR: Undefined Function\n";
	
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
				if(eval.tos > 1 || (temp >= 1 && temp <= 7))
				{
					switch((int)(temp))
					{
						case 1://unary -
							b = pop(&eval);
							x = -1*b;
							push(&eval,x);
							break;
						case 2://cosine function
							b = pop(&eval);
							x = cos(b);
							push(&eval,x);
							break;
						case 3://cotangent function
							b = pop(&eval);
							x = cos(b);
							if(x == 0)
							{
								error = true;
								done = true;
								errorMsg = divideByZero;
							}
							else
							{
								x = tan(b);
								if(x == 0)
								{
									error = true;
									done = true;
									errorMsg = divideByZero;
								}
								else
								{
									x = 1/x;
									push(&eval,x);
								}
							}
							break;
						case 4://ln function
							b = pop(&eval);
							if(b <= 0)
							{
								error = true;
								done = true;
								errorMsg = undefinedFunction;
							}
							else
							{
								x = log(b);
								push(&eval,x);
							}
							break;
						case 5://log function
							b = pop(&eval);
							if(b < 0)
							{
								error = true;
								done = true;
								errorMsg = undefinedFunction;
							}
							else
							{
								x = log10(b);
								push(&eval,x);
							}
							break;
						case 6://sine function
							b = pop(&eval);
							x = sin(b);
							push(&eval,x);
							break;
						case 7://tangent function
							b = pop(&eval);
							x = cos(b);
							if(x == 0)
							{
								error = true;
								done = true;
								errorMsg = divideByZero;
							}
							else
							{
								x = tan(b);
								push(&eval,x);
							}
							break;
						case 42://multiplication
							b = pop(&eval);
							a = pop(&eval);
							x = a*b;
							push(&eval,x);
							break;
						case 43://addition
							b = pop(&eval);
							a = pop(&eval);
							x = a+b;
							push(&eval,x);
							break;
						case 45://subtraction
							b = pop(&eval);
							a = pop(&eval);
							x = a-b;
							push(&eval,x);
							break;
						case 47://division
							b = pop(&eval);
							a = pop(&eval);
							if(b != 0)
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
						case 94://exponent
							b = pop(&eval);
							a = pop(&eval);
							x = pow(a,b);
							push(&eval,x);
							break;
					}
				}
				if(index->s1.tos > 0)
				{
					i = (int)dequeue(index);
				}
			}
			qIndex++;
		}
	}
	if(!error)//if no errors, print answer
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