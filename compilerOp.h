#include<iostream>
#include "operations.h"
using namespace std;

struct SymbolTable* buildSymbolTable(FILE* f,char* memory)
{
	struct SymbolTable* root = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));
	root = NULL;
	struct SymbolTable* prev = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));
	prev = NULL;
	char* data = (char*)malloc(500);
	int block = 0;
	while (fgets(data,500,f))
	{
		struct SymbolTable* newNode = (struct SymbolTable*)malloc(sizeof(struct SymbolTable));
		newNode->next = NULL;
		if (data[0] == 'S')
			return root;
		if ((data[0] == 'D'&&isEqual(data, "DATA")) || (data[0] == 'C'&&isEqual(data, "CONST")))
		{
			char c = 'D';
			if (data[0] == 'C')
			{
				c = 'C';
				data += 6;
			}
			else
				data += 5;
			data=removeSpaces(data);
			newNode->var = data[0];
			newNode->addr = block;
			if (data[1] == '[')
				newNode->size = toInt(data+2);
			else
				newNode->size = 1;
			if (c == 'C')
			{
				while (data[0] != '=')
					data += 1;
				data += 1;
				data=removeSpaces(data);
				memory[block] =toInt(data);
			}
			block += newNode->size;
			if (root == NULL)
			{
				root = newNode;
				prev = newNode;
			}
			else
			{
				prev->next = newNode;
				prev = newNode;
			}
		}
	}
}

void generateOpCode(FILE* f, struct SymbolTable* st_root, struct BlockAddr** ba_root,struct InterCode** ic_root)
{
	FILE* wf = fopen("code.txt", "w");
	int instNum = 1;
	
	int* stack=(int*)malloc(sizeof(int)*100);
	int top = -1;
	
	char* data = (char*)malloc(500);
	
	struct InterCode* ic_prev = (struct InterCode*)malloc(sizeof(struct InterCode));
	ic_prev = NULL;

	struct BlockAddr* ba_prev = (struct BlockAddr*)malloc(sizeof(struct BlockAddr));
	ba_prev = NULL;
	
	while (fgets(data,500,f)!=NULL)
	{
		data = removeSpaces(data);
		
		if (isEqual(data, "MOV"))
		{
			struct InterCode* newNode = mov(data,st_root,instNum);
			ic_prev->next = newNode;
			ic_prev = newNode;
			instNum++;
		}
		
		else if (isEqual(data, "ADD") || isEqual(data, "SUB") || isEqual(data, "MUL"))
		{
			struct InterCode* newNode = arithmeticOperation(data, instNum);
			if (data[0] == 'A')
				newNode->opCode = 3;
			else if (data[0] == 'S')
				newNode->opCode = 4;
			else
				newNode->opCode = 5;
			ic_prev->next = newNode;
			ic_prev = newNode;
			instNum++;
		}
		
		else if (isEqual(data, "READ") || isEqual(data, "PRINT"))
		{
			struct InterCode* newNode = IO(instNum);
			data = removeSpaces(data);
			if (data[0] == 'R')
			{
				data += 5;
				data = removeSpaces(data);
				newNode->opCode = 14;
				newNode->op1 = data[0] - 'A';
			}
			else
			{
				data += 6;
				data = removeSpaces(data);
				newNode->opCode = 13;
				if (data[1] != '[')
					newNode->op1= find(data[0],st_root);
				else
					newNode->op1= find(data[0], st_root) + toInt(data + 2);
			}
			if ((*ic_root) == NULL)
			{
				(*ic_root) = newNode;
				ic_prev = newNode;
			}
			else
			{
				ic_prev->next = newNode;
				ic_prev = newNode;
			}
			instNum++;
		}
		else if (data[1] == ':')
		{
			struct BlockAddr* newNode = label(data, instNum);
			if ((*ba_root) == NULL)
			{
				(*ba_root) = newNode;
				ba_prev = newNode;
			}
			else
			{
				ba_prev->next = newNode;
				ba_prev = newNode;
			}
		}
		else if (isEqual(data, "IF"))
		{
			struct InterCode* newNode = processIf(data,instNum,stack,&top);
			ic_prev->next = newNode;
			ic_prev = newNode;
			instNum++;
		}
		else if (isEqual(data, "ELSE"))
		{
			struct InterCode* newNode = processElse(instNum, stack, &top);
			ic_prev->next = newNode;
			ic_prev = newNode;
			instNum++;
			struct BlockAddr* newBlock = (struct BlockAddr*)malloc(sizeof(struct BlockAddr));
			newBlock->var[0] = 'E';
			newBlock->var[1] = 'L';
			newBlock->var[2] = 'S';
			newBlock->var[3] = 'E';
			newBlock->var[4] = '\0';
			newBlock->addr = instNum;
			newBlock->next = NULL;
			ba_prev->next = newBlock;
			ba_prev = newBlock;
		}
		else if (isEqual(data, "JUMP"))
		{
			struct InterCode* newNode = jump(*ba_root, data, instNum);
			ic_prev->next = newNode;
			ic_prev = newNode;
			instNum++;
		}
		else if (isEqual(data, "ENDIF"))
			endIf(*ic_root, stack,&top,instNum);
	}
}


void writeToFile(struct SymbolTable* st_root, struct BlockAddr* ba_root, char* memory, struct InterCode* ic_root)
{
	FILE* opCode = fopen("opcode.txt", "w");
	writeSt(st_root,opCode);
	writeBa(ba_root,opCode);
	writeMemory(memory,opCode);
	writeIc(ic_root,opCode);
	printf("Your opcode has been Written to: opcode.txt\n");
	fclose(opCode);
}