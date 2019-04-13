#include "tlb.h"

void tlb_traversal(TLB *tlbhead)
{
    if (tlbhead == NULL)
    {
        return;
    }
    TLB *p;
    unsigned i = 0;
    for (p = tlbhead; p != NULL; p = p->next)
    {
        i++;
        #if defined(DEBUG)
            printf("%u. pgnum: %lu, miscounts: %u\n", i, p->pgnum, p->miscounts);
        #endif
    }
}

void evict(TLB *tlb, unsigned long newpgnum)
{
    #if defined(DEBUG)
        printf("[0]evict %lu by %lu\n", tlb->pgnum, newpgnum);
    #endif
    //tlb->hitcounts = 0;
    tlb->pgnum = newpgnum;
}

TLB *search_by_pgnum(TLB *tlbhead, unsigned long pgnum)
{
    int flag = 0;
    if (tlbhead == NULL)
    {
        return NULL;
    }
    TLB *tlb;
    for (tlb = tlbhead; tlb != NULL; tlb = tlb->next)
    {
        if (tlb->pgnum == pgnum)
        {
            flag = 1;
            //tlb->miscounts=1;
            #if defined(DEBUG)
                printf("[+]hit %lu\n", tlb->pgnum);
            #endif
            return tlb;
        }
    }
    if (flag == 0)
    {
        return NULL;
    }
}

void add_all_miscounts(TLB *tlbhead)
{
    if (tlbhead == NULL)
    {
        return;
    }
    TLB *tlb;
    for (tlb = tlbhead; tlb != NULL; tlb = tlb->next)
    {
        tlb->miscounts++;
    }
}

TLB *LRU_search(TLB *tlbhead)
{
    if (tlbhead == NULL)
    {
        return NULL;
    }
    TLB *p, *evict_tlb;
    unsigned int max = tlbhead->miscounts;
    evict_tlb = tlbhead;
    for (p = tlbhead; p != NULL; p = p->next)
    {
        if (p->miscounts > max)
        {
            max = p->miscounts;
            evict_tlb = p;
        }
    }
    return evict_tlb;
}

void empty_tlbs(TLB **tlbhead)
{
    if (*tlbhead == NULL)
    {
        return;
    }
    TLB *p1 = *tlbhead;
    TLB *p2;
    while (p1->next != NULL)
    {
        p2 = p1->next;
        free(p1);
        p1 = p2;
    }
    free(p1);
    *tlbhead = NULL;
}