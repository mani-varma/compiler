#include<iostream>
//#include<stdio.h>
#include "compilerOp.h"
using namespace std;

int main()
{
	char* file = (char*)malloc(100);
	printf("Enter Program File:");
	scanf("%s",file);
	FILE* f = fopen(file, "r");
	
	
	char* memory = (char*)malloc(100);
	for (int i = 0; i < 100; i++)
		memory[i] = '_';
	
	
	struct SymbolTable* st_root = buildSymbolTable(f,memory);
	struct BlockAddr**ba_root = (struct BlockAddr**)malloc(sizeof(struct BlockAddr*));
	struct InterCode** ic_root = (struct InterCode**)malloc(sizeof(struct InterCode*));
	
	*ic_root = (struct InterCode*)malloc(sizeof(struct InterCode));
	*ic_root = NULL;
	
	*ba_root = (struct BlockAddr*)malloc(sizeof(struct BlockAddr));
	*ba_root = NULL;


	generateOpCode(f,st_root, ba_root, ic_root);
	

	writeToFile(st_root, *ba_root, memory, *ic_root);
	system("pause");
	return 0;
}