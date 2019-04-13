#include <stdio.h>
#include <stdlib.h>

//DEBUG option,if defined,it will print detailed info
//#define DEBUG

typedef struct TLB
{
    unsigned long pgnum;
    //unsigned int hitcounts;
    /*
      The miscounts have different interpretations for diffrent policies.
      [1] For FIFO, miscounts indicates how many times a node stores in the linked list
      [2] For LRU, if the pgnum did not hit, than for every miscounts stored miscounts +1;
          elseif it hits, reset the correspoding miscounts to 0.
      The algorithm of LRU will find the node with maxinum miscounts, 
      and evict it right after.
    */
    unsigned int miscounts;
    struct TLB *next;
} TLB;

void tlb_traversal(TLB *tlbhead);

//evict old tlb nodes.  As a matter of fact, we only need to change its pgnum and hitcounts
void evict(TLB *tlb, unsigned long newpgnum);

TLB *search_by_pgnum(TLB *tlbhead, unsigned long pgnum);

//search the Least Recently Used node
TLB *LRU_search(TLB *tlbhead);

//make all nodes miscounts++
void add_all_miscounts(TLB *tlbhead);

//delete all tlb nodes
void empty_tlbs(TLB **tlbhead);