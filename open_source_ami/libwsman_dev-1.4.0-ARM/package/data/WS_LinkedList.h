/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * Filename : LinkedList.h
 * 
 * File description : Singly linked list implementation 
 *
 *  Author: Manish Tomar <manisht@amiindia.co.in> 
 ******************************************************************/
#ifndef _WS_LINKEDLIST_H
#define _WS_LINKEDLIST_H

#include "WS_Include.h"

typedef struct ListNode
{
	void *address;
	struct ListNode *next;
}ListNode;

typedef struct LinkedList
{
	ListNode *head;
}LinkedList;

extern LinkedList *CreateList();
extern WS_INT16 AddElement(LinkedList *list, void *address); // AddElement will add the element to the first
extern WS_INT16 AppendElement(LinkedList *list, void *address);// AppendElement will add the element to the last
extern WS_INT16 RemoveElement(LinkedList *list, void *address);
extern void DisplayList(LinkedList *list, void (*dispFunc)(void *address));
extern WS_INT16 FreeList(LinkedList *list);
extern void *GetElement(LinkedList *list,WS_INT16 (*comparingFunc)(void *address, void *data), void *data);
WS_INT16 InvokeOnEachItem(LinkedList *list, WS_INT16 (*utilFunc)(void *address));
extern WS_INT16 SortList(LinkedList *list,WS_INT16 (*sortingFunc) (void *currentproperty, void *nextproperty));
#endif //_WS_LINKEDLIST_H
