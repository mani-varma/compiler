#include<iostream>
#include "helperFuncs.h"
using namespace std;

struct InterCode* mov(char* data,struct SymbolTable* st_root,int no)
{
	data += 3;
	data = removeSpaces(data);
	struct SymbolTable* temp = st_root;
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	newNode->next = NULL;
	if (data[1] == 'X')
	{
		newNode->opCode = 2;
		newNode->op1 = data[0] - 'A';
		data=moveNextToComma(data);
		data = removeSpaces(data);
		if (data[1] != '[')
			newNode->op2 = find(data[0], temp);
		else
			newNode->op2 = find(data[0], temp) + toInt(data + 2);
	}
	else
	{
		newNode->opCode = 1;
		if (data[1] != '[')
			newNode->op1 = find(data[0], temp);
		else
			newNode->op1 = find(data[0], temp) + toInt(data + 2);
		data = moveNextToComma(data);
		data = removeSpaces(data);
		newNode->op2 = data[0] - 'A';
	}
	newNode->op3 = -1;
	newNode->op4 = -1;
	newNode->no = no;
	return newNode;
}

struct InterCode* arithmeticOperation(char* data,int no)
{
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	newNode->next = NULL;
	data += 3;
	data = removeSpaces(data);
	newNode->op1 = data[0] - 'A';
	data=moveNextToComma(data);
	data = removeSpaces(data);
	newNode->op2=data[0] - 'A';
	data=moveNextToComma(data);
	data=removeSpaces(data);
	newNode->op3=data[0] - 'A';
	newNode->op4 = -1;
	newNode->no = no;
	return newNode;
}

struct InterCode* IO(int no)
{
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	newNode->no = no;
	newNode->next = NULL;
	newNode->op2 = -1;
	newNode->op3 = -1;
	newNode->op4 = -1;
	return newNode;
}

struct InterCode* processIf(char* data, int no, int* stack, int* top)
{
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	data += 2;
	data = removeSpaces(data);
	newNode->opCode = 7;
	newNode->op1 = data[0] - 'A';
	data += 2;
	data = removeSpaces(data);
	if (isEqual(data, "EQ"))
	{
		newNode->op3 = 8;
		data += 2;
	}
	else if (isEqual(data, "LT"))
	{
		newNode->op3 = 9;
		data += 2;
	}
	else if (isEqual(data, "GT"))
	{
		newNode->op3 = 10;
		data += 2;
	}
	else if (isEqual(data, "LTEQ"))
	{
		newNode->op3 = 11;
		data += 4;
	}
	else if (isEqual(data, "GTEQ"))
	{
		newNode->op3 = 12;
		data += 4;
	}
	data = removeSpaces(data);
	newNode->op2 = data[0] - 'A';
	newNode->op4 = -1;
	newNode->next = NULL;
	newNode->no = no;
	stack[++(*top)] = no;
	return newNode;
}

struct InterCode* processElse(int no, int* stack, int* top)
{
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	newNode->no = no;
	newNode->opCode = 6;
	newNode->op1 = -1;
	newNode->op2 = -1;
	newNode->op3 = -1;
	newNode->op4 = -1;
	newNode->next = NULL;
	stack[++(*top)] = no;
	return newNode;
}

struct BlockAddr* label(char* data, int no)
{
	struct BlockAddr* newNode=(struct BlockAddr*)malloc(sizeof(struct BlockAddr));
	int i;
	for (i = 0; data[i] != ':'; i++)
		newNode->var[i] = data[i];
	newNode->var[i] = '\0';
	newNode->addr = no;
	newNode->next = NULL;
	return newNode;
}

struct InterCode* jump(struct BlockAddr* ba_root, char* data, int no)
{
	data += 4;
	data = removeSpaces(data);
	struct InterCode* newNode = (struct InterCode*)malloc(sizeof(struct InterCode));
	newNode->no = no;
	newNode->opCode = 6;
	newNode->op1 = findBlock(data, ba_root);
	newNode->op2 = -1;
	newNode->op3 = -1;
	newNode->op4 = -1;
	newNode->next = NULL;
	return newNode;
}

void endIf(struct InterCode* ic_root, int* stack, int * top,int instNum)
{
	struct InterCode* node = findInterCode(stack[(*top)--], ic_root);
	if (node->opCode == 6)
	{
		node->op1 = instNum;
		instNum = node->no + 1;
		struct InterCode* ifNode = findInterCode(stack[(*top)--], ic_root);
		ifNode->op4 = instNum;
	}
	else
		node->op4 = instNum;

}

void writeSt(struct SymbolTable* st_root, FILE* opCode)
{
	fprintf(opCode,"Symbol Table\n");
	fprintf(opCode, "Variable  Address  Size\n");
	while (st_root != NULL)
	{
		fprintf(opCode, "   %c       %d       %d\n", st_root->var, st_root->addr, st_root->size);
		st_root = st_root->next;
	}
}

void writeBa(struct BlockAddr* ba_root, FILE* opCode)
{
	fprintf(opCode,"\n\nBlock Address\n");
	if (ba_root == NULL)
	{
		fprintf(opCode, "Empty\n");
		return;
	}
	fprintf(opCode, "Label  Address\n");
	while (ba_root != NULL)
	{
		fprintf(opCode, "%s %d\n", ba_root->var, ba_root->addr);
		ba_root = ba_root->next;
	}
}

void writeMemory(char* memory, FILE* opCode)
{
	fprintf(opCode,"\n\nMemory\n");
	for (int i = 0; i < 100; i++)
	{
		if (memory[i] == '_')
			fprintf(opCode, "_ ");
		else
			fprintf(opCode, "%d ", memory[i]);
		if ((i+1) % 20 == 0)
			fprintf(opCode, "\n");
	}
}


void writeIc(struct InterCode* ic_root, FILE* opCode)
{
	fprintf(opCode, "\n\nINTERMEDIATE CODE\n");
	fprintf(opCode, "No-OpCode-p1-p2-p3-p4\n");
	while (ic_root != NULL)
	{
		fprintf(opCode, "%d-%d-%d", ic_root->no, ic_root->opCode, ic_root->op1);
		if (ic_root->op2 != -1)
			fprintf(opCode, "-%d", ic_root->op2);
		if (ic_root->op3 != -1)
			fprintf(opCode, "-%d", ic_root->op3);
		if (ic_root->op4 != -1)
			fprintf(opCode, "-%d", ic_root->op4);
		fprintf(opCode, "\n");
		ic_root = ic_root->next;
	}
}