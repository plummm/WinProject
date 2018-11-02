// GarbageOpcode.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#pragma warning(disable: 4996)

//char *flag = "flag{garbage_opcode_confuses_analyzing}";
char ooo[39] = { 0x29, 0x54, 0x4f, 0x57, 0x1, 0x1a, 0x10, 0x3d, 0x5a, 0x4f, 0x57, 0x1f, 0x22, 0x1e, 0x3f, 0x5b, 0x41, 0x54, 0x1f, 0x22, 0x12, 0x20, 0x56, 0x48, 0x45, 0x9, 0x18, 0x2, 0x10, 0x59, 0x40, 0x51, 0x16, 0x4, 0xb, 0x26, 0x56, 0x49, 0x4d };
char v1[13] = { 126, 38, 32, 57, 102, 58, 121, 83, 42, 58, 35, 108, 0 };
char v2[21] = { 108, 44, 44, 50, 109, 118, 117, 83, 46, 110, 119, 86, 86, 62, 72, 52, 51, 63, 108, 3, 0 };
char *key = "O8.0z}q";
char result[40];
char input[40];


void doWork()
{
	for (int i = 0; i < 12; i++)
	{
		v1[i] ^= ooo[i];
	}
	for (int i = 0; i < 20; i++)
	{
		v2[i] ^= ooo[i];
	}
	__asm
	{
		// Will always set zero flag
		xor eax, eax
		jz valid
		// Insert long jump opcode
		
		__asm __emit(0x58)
		__asm __emit(0x58)
		__asm __emit(0xea)
		__asm __emit(0x83)
		__asm __emit(0xc4)
		__asm __emit(0x1c)
		__asm __emit(0xc3)
		valid:
	}
	for (int i = 0; i < 39; i++)
	{
		result[i] = input[i] ^ key[i % 7];
		if (result[i] != ooo[i])
		{
			printf("%s\n",v1);
			return;
		}
		//printf("0x%x, ", result[i]);
	}
	printf("%s\n",v2);
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
		call doWork
	}
	system("pause");
	return 0;
}
