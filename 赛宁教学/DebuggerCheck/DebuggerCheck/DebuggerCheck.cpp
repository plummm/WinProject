// DebuggerCheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int CheckDebugger()
{
	__asm
	{
		mov eax, dword ptr fs : [0x18]
		//  Get PEB structure address
		mov eax, dword ptr[eax + 0x30]
		// Check if isDebug byte is set
		cmp byte ptr[eax + 2], 0
		je blocEnd
		// Debugger detected
		mov eax, 1
		jmp end
		blocEnd :
		mov eax, 0
		end:
		// etc ...
	}
}

int main()
{
	if (CheckDebugger())
	{
		printf("Find Debugger");
	}
	else
	{
		printf("Good person.");
	}
    return 0;
}

