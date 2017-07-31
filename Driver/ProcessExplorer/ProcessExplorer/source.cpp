#include "Driver.h"

VOID LinkList(printList* head, PVOID pIoBuffer)
{
	head->next = (printList*)pIoBuffer;
	pIoBuffer = (PVOID)head;
}