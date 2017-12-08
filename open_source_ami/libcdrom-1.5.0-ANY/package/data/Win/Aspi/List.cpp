////////////////////////////////////////////////////////////
//
// Module LIST.CPP
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

#include <stdio.h>
#include "list.h"

ItemLink::ItemLink()
{
   next = prev = this;
   item = NULL;
}

ItemLink::ItemLink(void *p)
{
   next = prev = this;
   item = p;
}


void *ItemLink::GetItem()
{
   return item;
}

void ItemLink::PrependLink(ItemLink *newlink)
{
   newlink->next = this;
   newlink->prev = prev;
   prev->next = newlink;
   prev = newlink;
}

void ItemLink::AppendLink(ItemLink *newlink)
{
   newlink->prev = this;
   newlink->next = next;
   next->prev = newlink;
   next = newlink;
}

void ItemLink::Unlink()
{
   prev->next = next;
   next->prev = prev;
}

ItemLink *ItemLink::GetNextLink()
{
   return next;
}

ItemLink *ItemLink::GetPrevLink()
{
   return prev;
}




int ItemList::IsEmpty()
{
   if (Link.GetNextLink() == &Link)
      return 1;
   else
      return 0;
}

ItemLink *ItemList::GetFirstLink()
{
   if (IsEmpty())
      return NULL;
   else
      return Link.GetNextLink();
}

ItemLink *ItemList::GetLastLink()
{
   if (IsEmpty())
      return NULL;
   else
      return Link.GetPrevLink();
}

void *ItemList::GetFirstItem()
{
   if (IsEmpty())
      return NULL;
   else
      return Link.GetNextLink()->GetItem();
}

void *ItemList::GetLastItem()
{
   if (IsEmpty())
      return NULL;
   else
      return Link.GetPrevLink()->GetItem();
}


int ItemList::AddItem(void *p)
{
   ItemLink *lp = new ItemLink(p);
   if (lp)
      {
      Link.PrependLink(lp);
      return 1;
      }
   else
      return 0;
}

int ItemList::RemoveItem(void *p)
{
   ItemLink *lp = GetFirstLink();
   while (lp && (lp != &Link))
      {
      if (lp->GetItem() == p)
         {
         lp->Unlink();
         delete lp;
         return 1;
         }
      lp = lp->GetNextLink();
      }
   return 0;
}

int ItemList::IsMember(void *p)
{
   ItemLink *lp = GetFirstLink();
   while (lp && (lp != &Link))
      {
      if (lp->GetItem() == p)
         return 1;
      lp = lp->GetNextLink();
      }
   return 0;
}


ItemList::ItemList()
{
}

ItemList::~ItemList()
{
   while (!IsEmpty())
      {
      ItemLink *p = GetFirstLink();
      p->Unlink();
      delete p;
      }
}



ItemListIterator::ItemListIterator()
{
   List = NULL;
   Current = NULL;
}

ItemListIterator::ItemListIterator(ItemList *p)
{
   List = p;
   Current = NULL;
}


void *ItemListIterator::GetFirst(ItemList *p)
{
   if (p)
      List = p;

   if (List)
      {
      Current = List->GetFirstLink();
      if (Current)
         return Current->GetItem();
      else
         return NULL;
      }
   return NULL;
}


void *ItemListIterator::GetCurrent()
{
   if (List)
      {
      if (!Current)
         {
         // Start at beginning of list
         Current = List->GetFirstLink();
         if (!Current)
            return NULL;
         }
      return Current->GetItem();
      }
   else
      return NULL;
}


void *ItemListIterator::IterateToNext()
{
   if (List)
      {
      if (!Current)
         {
         // Start at beginning of list
         Current = List->GetFirstLink();
         if (!Current)
            return NULL;
         }
      if (Current != List->GetLastLink())
         {
         Current = Current->GetNextLink();
         return Current->GetItem();
         }
      else
         return NULL;
      }
   else
      return NULL;
}



#if TEST_LINK_CPP

main()
{

   static char *strlist[5] = {
      "Str 1",
      "Str 2",
      "Str 3",
      "Str 4",
      "Str 5"  };

   ItemList list;
   ItemListIterator iterator;
   char *p;
   int i;


   cout << "Adding -------\n";
   for (i=0; i<5; i++)
      {
      list.AddItem( strlist[i] );
      cout << "Add '" << strlist[i] << "'\n";
      }

   cout << "Dumping -------\n";
   for ( p=(char *) iterator.GetFirst(&list); p; p=(char *) iterator.IterateToNext() )
      cout << p << "\n";

   cout << "Removing '" << strlist[0] << "' -------\n";
   list.RemoveItem( strlist[0] );
   for ( p=(char *) iterator.GetFirst(&list); p; p=(char *) iterator.IterateToNext() )
      cout << p << "\n";

   cout << "Removing '" << strlist[2] << "' -------\n";
   list.RemoveItem( strlist[2] );
   for ( p=(char *) iterator.GetFirst(&list); p; p=(char *) iterator.IterateToNext() )
      cout << p << "\n";

   cout << "Removing '" << strlist[4] << "' -------\n";
   list.RemoveItem( strlist[4] );
   for ( p=(char *) iterator.GetFirst(&list); p; p=(char *) iterator.IterateToNext() )
      cout << p << "\n";

   cout << "Adding '" << strlist[4] << "' -------\n";
   list.AddItem( strlist[4] );
   for ( p=(char *) iterator.GetFirst(&list); p; p=(char *) iterator.IterateToNext() )
      cout << p << "\n";

   for (i=0; i<5; i++)
      cout << "'" << strlist[i] << (list.IsMember(strlist[i]) ? "' found\n" : "'  not found\n");

   return 0;
}
#endif
