#include<iostream>
using namespace std;

struct SymbolTable
{
	char var;
	int addr;
	int size;
	struct SymbolTable* next;
};

struct InterCode
{
	int no;
	int opCode;
	int op1;
	int op2;
	int op3;
	int op4;
	struct InterCode* next;
};

struct BlockAddr
{
	char var[10];
	int addr;
	struct BlockAddr* next;
};

int isEqual(char* command, char* original)
{
	int i;
	for (i = 0; command[i] != ' '&&command[i] != '\0'&&command[i] != '\n'&&original[i] != '\0'; i++)
		if (command[i] != original[i])
			return 0;
	if ((command[i] == ' ' || command[i] == '\0' || command[i] == '\n') && original[i] == '\0')
		return 1;
	return 0;
}

char* removeSpaces(char* data)
{
	while (data[0] == ' ')
		data=data+1;
	return data;
}

char* moveNextToComma(char* data)
{
	while (data[0] != ',')
		data += 1;
	data += 1;
	return data;
}

int find(char a, struct SymbolTable* root)
{
	struct SymbolTable* temp = root;
	while (temp != NULL)
	{
		if (temp->var == a)
			return temp->addr;
		temp = temp->next;
	}
	return -1;
}

int findBlock(char* block,struct BlockAddr* ba_root)
{
	struct BlockAddr* temp = ba_root;
	while (temp != NULL)
	{
		if(isEqual(block, temp->var))
			return temp->addr;
		temp = temp->next;
	}
	return -1;
}

struct InterCode* findInterCode(int no, struct InterCode* ic_root)
{
	struct InterCode* temp = ic_root;
	while (temp != NULL)
	{
		if (temp->no == no)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

int toInt(char* a)
{
	int n = 0;
	int i=0;
	while (a[i]>='0'&&a[i]<='9')
	{
		n = (n * 10) + (int(a[i]) - int('0'));
		i++;
	}
	return n;
}

