// DetectBreakpoint.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma warning(disable: 4996)

//char *flag = "flag{garbage_opcode_confuses_analyzing}";
char ooo[39] = { 0x29, 0x54, 0x4f, 0x57, 0x1, 0x1a, 0x10, 0x3d, 0x5a, 0x4f, 0x57, 0x1f, 0x22, 0x1e, 0x3f, 0x5b, 0x41, 0x54, 0x1f, 0x22, 0x12, 0x20, 0x56, 0x48, 0x45, 0x9, 0x18, 0x2, 0x10, 0x59, 0x40, 0x51, 0x16, 0x4, 0xb, 0x26, 0x56, 0x49, 0x4d };
char *key = "O8.0z}q";
char result[40];
char input[40];


void doWork()
{
	for (int i = 0; i < 39; i++)
	{
		result[i] = input[i] ^ key[i % 7];
		if (result[i] != ooo[i])
		{
			printf("Wrong inputs");
			return;
		}
		//printf("0x%x, ", result[i]);
	}
}

void detected()
{
	printf("Detect breakpoint!");
}

int main()
{
	scanf("%40s", input);
	__asm
	{
		// Will always set zero flag
		xor eax, eax
		jz valid
		// Insert long jump opcode
		__asm __emit(0x58)
		__asm __emit(0x58)
		__asm __emit(0xea)
		valid:
		xor ebx, ebx
		mov bl, 0xCC
		mov eax, doWork
		mov ecx, 0x9C
		antiBpLoop:
		cmp byte ptr[eax], bl
		je stop
		inc eax
		dec ecx
		jnz antiBpLoop
		call doWork
		jmp bpcontinue
		stop:
		call detected
		bpcontinue:
	}
	printf("Excellent! It works!");
	return 0;
}

