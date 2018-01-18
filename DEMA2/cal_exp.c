#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>

/* Using ARRAY to implement queue and stack */
/* MAX_TERM is array size */
#define	MAX_TERM	100
#define	NEXT_TERM(idx)	(((idx) + 1) % MAX_TERM)

void
FAIL_EXIT(int line_number, const char *func_name, char *msg)
{
	fprintf(stderr, "LN(%d): %s() %s\n", line_number, func_name, msg);
	exit(-1);
}

// Choise 1/0 to enable/disable debug information output
//#if 1
#if 0
#define	JWD0(x)	printf x
#define	JWD1(x)	printf x
#define	JWD2(x) printf x
#define	SHOW_QUEUE(qq)	Show_Queue(qq, #qq)
#define	SHOW_STACK(ss)	Show_Stack(ss, #ss)
#define	SHOW_ELEMENT(ee)	Show_Element(ee)
#else
#define	JWD0(x)
#define	JWD1(x)
#define	JWD2(x)
#define	SHOW_QUEUE(qq)
#define	SHOW_STACK(ss)
#define	SHOW_ELEMENT(ee)
#endif

typedef	struct	element	element;
#define	TOKEN_VALUE		1
#define	TOKEN_OPERATOR	2
struct	element 
{
	int token_type;
	int token_value;
};

/* token_type 2: operator 定义运算符号的数字*/
#define	OPERATOR_LPAREN	0
#define	OPERATOR_RPAREN	1
#define	OPERATOR_ADD	2
#define	OPERATOR_SUB	3
#define	OPERATOR_MUL	4
#define	OPERATOR_DIV	5
#define	OPERATOR_MOD	6
#define	OPERATOR_DEF	7
char operators[] = "()+-*/%=";/*设定运算符号的权重*/
static int priority_in_stack[] = 
{
	 0, 19, 12, 12, 13, 13, 13, 0
};
static int priority_out_stack[] = 
{
	20, 19, 12, 12, 13, 13, 13, 0
};

typedef	struct	
{
	element	list[MAX_TERM];
	int	front;
	int	rear;
} element_queue;/*宣告一个队列*/

typedef	struct	
{
	element	list[MAX_TERM];
	int	top;
} element_stack;/*宣告一个栈*/

void
Show_Element(element *ee)/*输出ee中的内容是数字还是运算符*/
{
	assert(ee);
	switch (ee->token_type) 
	{
	case TOKEN_VALUE:
		printf("%d ", ee->token_value);
		break;
	case TOKEN_OPERATOR:
		printf("%c ", operators[ee->token_value]);
		break;
	default:
		FAIL_EXIT(__LINE__, __FUNCTION__, "unknown type");
	}
}

/*********************************************************************/

void
Queue_Initialize(element_queue *qq)
{
	assert(qq);	/* Make sure qq is not null *//*防止qq中是空值*/ 
	qq->front = qq->rear = 0;
}

int
Queue_Size(element_queue *qq)
{
	assert(qq);
	return (qq->rear + MAX_TERM - qq->front) % MAX_TERM;
}

element *
Queue_Insert(element_queue *qq, element *ee)/*插入队列中*/
{
	int next;
	assert(qq);
	assert(ee);
	JWD2(("Queue_Insert(%d:%d) ", qq->front, qq->rear));
	next = NEXT_TERM(qq->rear);
	if (next == qq->front) 
	{
		JWD0(("Queue full"));
		return NULL;
	}
	qq->list[qq->rear] = *ee;
	qq->rear = next;
	SHOW_ELEMENT(ee);
	JWD2(("(%d:%d)\n", qq->front, qq->rear));
	return ee;
}

element *Queue_Delete(element_queue *qq, element *ee)/*把qq中的内容删除并输出给ee*/ 
{
	assert(qq);
	assert(ee);
	JWD2(("Queue_Delete(%d:%d) ", qq->front, qq->rear));
	if (qq->front == qq->rear) 
	{
		JWD2(("Queue is empty\n"));
		return NULL;
	}
	*ee = qq->list[qq->front];	
	qq->front = NEXT_TERM(qq->front);
	SHOW_ELEMENT(ee);
	JWD2(("(%d:%d)\n", qq->front, qq->rear));
	return ee;
}

/*********************************************************************/

void
Stack_Initialize(element_stack *ss)/*防止ss值为零*/
{
	assert(ss);
	ss->top = -1;
}

#define	STACK_TOP(ss)	((ss)->list[(ss)->top])

int
Stack_Size(element_stack *ss)
{
	assert(ss);
	return ss->top + 1;
}

element *
Push(element_stack *ss, element *ee)/*将ee加入到ss*/
{
	assert(ss);
	assert(ee);
	JWD2(("Push(%d) ", ss->top));
	if (ss->top >= MAX_TERM - 1) {
		JWD0(("Stack full"));
		return NULL;
	}
	ss->list[++(ss->top)] = *ee;
	SHOW_ELEMENT(ee);
	JWD2(("(%d)\n", ss->top));
	return ee;
}

element *
Pop(element_stack *ss, element *ee)/*将ss的值给予ee，回传ee*/
{
	assert(ss);
	assert(ee);
	JWD2(("Pop(%d) ", ss->top));
	if (ss->top < 0) 
	{
		JWD2(("Stack is empty\n"));
		return NULL;
	}
	*ee = ss->list[(ss->top)--];	
	SHOW_ELEMENT(ee);
	JWD2(("(%d)\n", ss->top));
	return ee;
}

/*********************************************************************/

void
Show_Queue(element_queue *qq, char *name)/*输出Queue中的运算符或者数字*/
{
	int ii, size;
	assert(qq);
	printf("Queue(%s:%d): ", name, size = Queue_Size(qq));
	for (ii = qq->front; size > 0; ii = NEXT_TERM(ii), size--) 
	{ 
		Show_Element(&(qq->list[ii]));/*输出qq中的内容*/
	}
	printf("\n");
}

void
Show_Stack(element_stack *ss, char *name)/*输出ss中的数字或者运算符*/
{
	int ii;
	assert(ss);
	printf("Stack(%s:%d): ", name, Stack_Size(ss));
	for (ii = 0; ii <= ss->top; ii++) 
	{
		Show_Element(&(ss->list[ii]));
	}
	printf("\n");
}

/*********************************************************************/

int
ToPostfix(element_queue *infix, element_queue *postfix)/*中置转后置*/
{
	element_stack	stack;
	element_stack	stack2;
	element	ee;

	Stack_Initialize(&stack);/*初始化stack postfix*/
	Queue_Initialize(postfix);
    element_queue outfix;
	SHOW_QUEUE(infix);
	while (Queue_Delete(infix, &ee)) /*把infix的内容给ee*/
	{	
	    
		SHOW_QUEUE(infix);
		if (ee.token_type != TOKEN_OPERATOR) 
		{
			if (! Queue_Insert(postfix, &ee)) /*排除0，将数字输入postfix*/
			{
				return -1;
			}
			SHOW_QUEUE(postfix);/*输出数字*/
		} 
		else 
		{/*如果输入ee的是运算符号*/
			while (Stack_Size(&stack)) /*如果Stack有符号存在*/
			{/*stack顶端的in权重小于ee中元素out的权重*/
					
				if (priority_in_stack[STACK_TOP(&stack).token_value] < priority_out_stack[ee.token_value]) 
					{
					/*	if( priority_out_stack[ee.token_value]==19)
						{
							Push(&stack2, &ee);
							break;
						}
						else*/
						break;/*跳出WHILE*/
				    } 
				
				else //当stack顶端的in权重大于等于ee中元素out的权重
				{/*输出加减乘除*/
				    
						
					element e1;
					
					Pop(&stack, &e1);/*弹出stack中的符号给e1*/	
					
					if (! Queue_Insert(postfix, &e1)) /*将e1中的符号给postfix*/
					{
						return -1;
					}
					SHOW_STACK(&stack); SHOW_QUEUE(postfix);
				}
			}
			
			if(&ee!=0)
			{
				if(priority_out_stack[ee.token_value]==19)
				{
					Push(&stack2, &ee);
					while(priority_in_stack[STACK_TOP(&stack).token_value]!=0)
					{
						Pop(&stack, &ee);
						Queue_Insert(postfix, &ee);
					}
					while(priority_in_stack[STACK_TOP(&stack).token_value]==0)
					{
						element e2;
						Pop(&stack, &e2);
					}
				}
				else
				{
					Push(&stack, &ee);
				}
			}
			
/*			if (! Push(&stack, &ee)) //如果ee不为0 将ee中的符号先存入stack
			{
				return -1;
			}*/
			SHOW_STACK(&stack);
		}
	}
	
/*将剩余的符号移出队列*/	
    while (Pop(&stack, &ee)) /*输出stack中的内容到ee*/
	{
		if (! Queue_Insert(postfix, &ee)) /*将ee加到postfix中*/
		{
			return -1;
		}
		SHOW_STACK(&stack); SHOW_QUEUE(postfix);	// should be empty
	}
	return Queue_Size(postfix);
}

int
Compute(int op, int aa, int bb, int *value)
{
	switch (op) /*分析op的类型，运算aa和bb 不需要更改*/
	{
	case	OPERATOR_ADD: *value = aa + bb; return 0;
	case	OPERATOR_SUB: *value = aa - bb; return 0;
	case	OPERATOR_MUL: *value = aa * bb; return 0;
/*	case	OPERATOR_LPAREN	:  return -1;
	case    OPERATOR_RPAREN	:   return -1;
*/	
	case	OPERATOR_DIV:
		if (bb) 
		{
			*value = aa / bb; return 0;
		} 
		else 
		{
			printf("divide by error\n");
			return -1;
		}
	case	OPERATOR_MOD:
		if (bb) 
		{
			*value = aa % bb; return 0;
		} 
		else 
		{
			printf("divide by error\n");
			return -1;
		}
	}
}

int
Evaluate(element_queue *postfix, int *ret)
{                                            /*分析输入的字串*/
	element_stack stack;
	element	ee;

	Stack_Initialize(&stack);/*初始化stack*/
	SHOW_QUEUE(postfix);
	while (Queue_Delete(postfix, &ee)) /*进行postfix中的内容输出给ee*/
	{
		SHOW_QUEUE(postfix);
		if (ee.token_type != TOKEN_OPERATOR)/*如果ee中的token type不是运算符号*/
		{
			if (! Push(&stack, &ee)) /*把ee数字给stack*/
			{
				return -1;
			}
			SHOW_STACK(&stack);
		} 
		
		else /*ee中的是运算符号时*/
		{
			int vv;
			element e2, e1;
			
			if (! Pop(&stack, &e2) || ! Pop(&stack, &e1)) /*如果无法pop出stack里的内容*/
			{
				printf("wrong expression\n");
				return -1;
			}
			 
			else /*如果可以将stack中的内容给e2，e1*/
			{   
			    
				if (Compute(ee.token_value, e1.token_value, e2.token_value, &vv) < 0) 
				/*开始运算ee算法的e1和e2的，判断结果如果小于0*/ 
				{
					return -1;
				}
				e1.token_value = vv;
				Push(&stack, &e1);/*赋予stack e1的值*/
				SHOW_STACK(&stack);	SHOW_QUEUE(postfix);
				
			}
		}
	}
	if (! Pop(&stack, &ee)) /*pop出的ee为0*/
	{
		printf("wrong expression\n");
		return -1;
	} 
	else 
	{
		SHOW_STACK(&stack);	SHOW_QUEUE(postfix);
		*ret = ee.token_value;
		if (Stack_Size(&stack) == 0) 
		{
			return 0;
		} 
		else 
		{
			printf("wrong expression\n");
			return -1;
		}
	}
}

/* Token Analysis *//*分析Token中的内容为数字或是运算符号*/ 
int
analysis_token(char *token, int *token_value)
{
	char *ch;
	JWD1(("Token = %s\n", token));
	if (isdigit(token[0])) 
	{
		*token_value = atoi(token);/*如果是数字，输出token value为这个数字*/
		return TOKEN_VALUE;
	} 
	else if (ch = strchr(operators, token[0])) 
	{
		*token_value = ch - operators;
		return TOKEN_OPERATOR;
	} 
	else 
	{
		printf("unknown token [%s]\n", token);
		return -1;
	}
}

int
Parse(char *buf, element_queue *infix)/*分析输入的字串*/
{
	char *token;
	element ee;
	int ne = 0;	/* number of elements */

	Queue_Initialize(infix);

	if (! (token = strtok(buf, " "))) 
	{	/* get first token 如果输入的数字没有用空格隔开，则输出0*/
		return ne;
	}
	do {
		if ((ee.token_type = analysis_token(token, &ee.token_value)) < 0) 
		{
			return -1;
		};
		if (! Queue_Insert(infix, &ee)) 
		{
			return -1;
		}
		ne++;
	} while (token = strtok(NULL, " "));	/* get next token */

	assert(ne == Queue_Size(infix));
	return ne;
}

int
main(int argc, char *argv[])
{
	element_queue infix, postfix;
	char buffer[4096];
	char *buf;
	int ret;

	while (buf = gets(buf=buffer)) 
	{
		printf("%s = ", buf);
		if (Parse(buf, &infix) <= 0) 
		{ 
			/* No token (empty line) or Error */
			continue;
		}
		if (ToPostfix(&infix, &postfix) < 0) 
		{ 
			continue;
		}
		if (Evaluate(&postfix, &ret) < 0) 
		{
			continue;
		}
		printf("%d\n", ret);
	}
	return 0;
}

