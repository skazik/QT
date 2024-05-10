#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "llist.h"

#ifndef NULL
#define NULL (0)
#endif

llist_t* newlist(void)
{
   llist_t* l =  (llist_t*) malloc(sizeof(llist_t));
   if (NULL == l)
   {
       printf("error allocation\n");
       exit(-1);
   }

   l->head = NULL;
   l->current = NULL;
    return l;
}
// display the list
void printList(llist_t* l)
{
   struct node *p = l->head;
   printf("list(%08X): [", (unsigned int) l);

   //start from the beginning
   while(p != NULL) {
      printf(" %d ", p->data);
      p = p->next;
   }
   printf("]\n");
}

//insertion at the beginning
void insertatbegin(llist_t *l, int data)
{
   //create a link
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->data = data;

   // point it to old first node
   lk->next = l->head;

   //point first to new first node
   l->head = lk;
}
void insertatend(llist_t *l, int data)
{

    if (!l->head && !l->current)
    {
        insertatbegin(l, data);
        return;
    }
   //create a link
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->data = data;
   struct node *linkedlist = l->head;

   // point it to old first node
   while(linkedlist->next != NULL)
      linkedlist = linkedlist->next;

   //point first to new first node
   linkedlist->next = lk;
}
void insertafternode(struct node *list, int data)
{
   struct node *lk = (struct node*) malloc(sizeof(struct node));
   lk->data = data;
   lk->next = list->next;
   list->next = lk;
}

void deleteatbegin(llist_t* l)
{
   l->head = l->head->next;
}

void deleteatend(llist_t *l)
{
   struct node *linkedlist = l->head;
   while (linkedlist->next->next != NULL)
      linkedlist = linkedlist->next;
   linkedlist->next = NULL;
}

void deletenode(llist_t *l, int key)
{
   struct node *temp = l->head, *prev;
   if (temp != NULL && temp->data == key) {
      l->head = temp->next;
      return;
   }

   // Find the key to be deleted
   while (temp != NULL && temp->data != key) {
      prev = temp;
      temp = temp->next;
   }

   // If the key is not present
   if (temp == NULL) return;

   // Remove the node
   prev->next = temp->next;
}

int searchlist(llist_t *l, int key)
{
   struct node *temp = l->head;
   while(temp != NULL) {
      if (temp->data == key) {
         return 1;
      }
      temp=temp->next;
   }
   return 0;
}
void lltest()
{
    llist_t *l = newlist();
    if (!l)
        exit(-1);

   int k=0;
   insertatbegin(l, 12);
   insertatbegin(l, 22);
   insertatend(l, 30);
   insertatend(l, 44);
   insertatbegin(l, 50);
   insertafternode(l->head->next->next, 33);
   printf("Linked List: ");

   // print list
   printList(l);
   deleteatbegin(l);
   deleteatend(l);
   deletenode(l, 12);
   printf("\nLinked List after deletion: ");

   // print list
   printList(l);
   insertatbegin(l, 4);
   insertatbegin(l, 16);
   printf("\nUpdated Linked List: ");
   printList(l);
   k = searchlist(l, 16);
   if (k == 1)
      printf("\nElement is found");
   else
      printf("\nElement is not present in the list");
}
