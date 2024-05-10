#ifdef __cplusplus
extern "C" {
#endif


struct node {
   int data;
   struct node *next;
};

typedef struct _llist {
    struct node *head;
    struct node *current;
} llist_t;

llist_t* newlist(void);

extern void printList(llist_t* l);
extern void insertatbegin(llist_t* l, int data);
extern void insertatend(llist_t* l, int data);
extern void deleteatbegin(llist_t* l);
extern void deleteatend(llist_t* l);
extern void deletenode(llist_t* l, int key);
extern int searchlist(llist_t* l, int key);
extern void lltest();

#ifdef __cplusplus
}
#endif
