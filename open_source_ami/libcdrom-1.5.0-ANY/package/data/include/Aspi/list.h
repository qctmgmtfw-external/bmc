////////////////////////////////////////////////////////////
//
// Module LIST.HPP
//
// ASPI class library
// Linked list support
//
// Project: A Programmer's Guide to SCSI
//
// Copyright (C) 1997, Brian Sawert
// Portions copyright (C) 1995, Larry Martin
// All rights reserved
//
////////////////////////////////////////////////////////////


#ifndef LIST_HPP_INCLUDED
#define LIST_HPP_INCLUDED


class ItemLink
   {
   public:
   ItemLink *next;
   ItemLink *prev;
   void *item;

   ItemLink();
   ItemLink(void *item);

   void *GetItem();

   void PrependLink(ItemLink *newlink);
   void AppendLink(ItemLink *newlink);
   void Unlink();

   ItemLink *GetNextLink();
   ItemLink *GetPrevLink();
   };


class ItemList
   {
   public:
   ItemLink Link;

   ItemList();
   ~ItemList();

   int IsEmpty();

   ItemLink *GetFirstLink();
   ItemLink *GetLastLink();

   void *ItemList::GetFirstItem();
   void *ItemList::GetLastItem();

   int AddItem(void *p);
   int RemoveItem(void *p);
   int IsMember(void *p);
   };

class ItemListIterator
   {
   public:
   ItemList *List;
   ItemLink *Current;

   ItemListIterator();
   ItemListIterator(ItemList *list);

   void *GetFirst(ItemList *list = NULL);
   void *GetCurrent();
   void *IterateToNext();
   };



#endif   /* LIST_HPP_INCLUDED */


