#include "main.h"

unsigned int g_pgsize = -1;
unsigned int g_tlbsize = -1;
unsigned int g_flushperiod = -1;
unsigned int g_flushcounts = 0;
int g_iflag = 0;
int g_pflag = -1; //0->FIFO 1->LRU

unsigned int g_totalhandled = 0;
unsigned int g_totalmisses = 0;
unsigned int g_totalhits = 0;

TLB *g_tlbhead = NULL;
TLB *g_tlbtail = NULL;
TLB *g_fifo_pointer = NULL;

unsigned int g_tlb_node_counts;


int main(int argc, char *argv[])
{
    extern char *optarg;
    int ch;
    int count_op = 0;
    while ((ch = getopt(argc, argv, "if:p:")) != -1)
    {
        switch (ch)
        {
        case 'i':
            #if defined(DEBUG)
                printf("have i\n");
            #endif
            g_iflag = 1;
            count_op++;
            break;
        case 'f':
            #if defined(DEBUG)
                printf("The flushperiod is %s\n", optarg);
            #endif
            g_flushperiod = (unsigned int)atoi(optarg);
            count_op++;
            break;
        case 'p':
            #if defined(DEBUG)
                printf("The argument of -p is %s\n", optarg);
            #endif
            if (strcmp(optarg, "FIFO") == 0)
            {
                #if defined(DEBUG)
                    printf("FIFO!\n");
                #endif
                g_pflag = 0;
            }
            else if (strcmp(optarg, "LRU") == 0)
            {
                #if defined(DEBUG)
                    printf("LRU!\n");
                #endif
                g_pflag = 1;
            }
            count_op++;
            break;
        }
    }
    #if defined(DEBUG)
        printf("count_op:%d, argc: %d\n", count_op, argc);
        printf("[+]pgsize:%s, tlbsize:%s\n", argv[argc - 2], argv[argc - 1]);
    #endif
    g_pgsize = (unsigned int)atoi(argv[argc - 2]);
    g_tlbsize = (unsigned int)atoi(argv[argc - 1]);

    char buf[1024];
    int i = 0;

    char testdata[10][1024] = {
        "I  00022305,2\n",
        "I  0000C790,2\n",
        " L 0003199C,4\n",
        " S 00004962,1\n",
    };

    while (1)
    {
        fgets(buf, sizeof buf, stdin);

        //for(i=0;i<4;i++){
        //strcpy(buf,testdata[i]);

        if (buf[0] == '=' && buf[1] == '=')
        {
            if (g_totalhandled == 0)
                continue;
            else
                break;
        }
        if (buf[strlen(buf) - 1] == '\n')
        {
            // read full line
            //printf("%s", buf);
            char ISLM;
            unsigned long addr;
            unsigned int size;
            parse_valgrind(buf, &ISLM, &addr, &size);
            //printf("%c %lu %d\n", ISLM, addr, size);

            if (g_iflag == 1 && ISLM == 'I')
            {
                flush(); //make sure flush() will be the end of loop
                continue;
            }

            //size across the page
            if(addr/g_pgsize!=(addr+size)/g_pgsize){
                handle_tlb(addr/g_pgsize);
                handle_tlb((addr+size)/g_pgsize);
            }else
            {
                handle_tlb(addr/g_pgsize);
            }
            
        }

    }
    
    printf("%u\n%u\n%u\n",g_totalhandled,g_totalmisses,g_totalhits);
}

void parse_valgrind(const char *line, char *ISLM, unsigned long *addr, unsigned int *size)
{
    if (line[0] != ' ')
    {
        *ISLM = line[0];
    }
    else
    {
        *ISLM = line[1];
    }
    int hexcounts = 0; //04caeaad:hexcounts=8; 1fff000118:hexcounts=10
    int i = 3;
    char temp_addr[16];
    bzero(temp_addr, sizeof(temp_addr));
    while (line[i++] != ',')
    {
        hexcounts++;
    }

    for (i = 3; i < hexcounts + 3; i++)
    {
        temp_addr[i - 3] = line[i];
    }
    //convert hex to dec
    *addr = 0;
    for (i = 0; i < hexcounts; i++)
    {
        if (temp_addr[i] >= '0' && temp_addr[i] <= '9')
        {
            *addr += (unsigned long)((temp_addr[i] - '0')) << (unsigned long)(4 * (hexcounts - 1 - i)); //i=0,ss[i]='2',*addr+=2<<8
        }
        if (temp_addr[i] >= 'a' && temp_addr[i] <= 'f')
        {
            *addr += (unsigned long)((temp_addr[i] - 'a' + 10)) << (unsigned long)(4 * (hexcounts - 1 - i));
        }
    }
    char temp_size[8];
    int sizecounts = 0; //8:sizecounts=1; 10:sizecounts=2
    bzero(temp_size, sizeof(temp_size));
    i = hexcounts + 4;
    while (line[i] != '\n')
    {
        temp_size[i - hexcounts - 4] = line[i];
        i++;
        sizecounts++;
    }
    //convert char* temp_size to dec
    *size = 0;
    for (i = 0; i < sizecounts; i++)
    {
        *size += (temp_size[i] - '0') * pow(10.0, (double)(sizecounts - 1 - i));
    }

    #if defined(DEBUG)
        printf("[*]handle %s...\n", temp_addr);
    #endif
}

void flush()
{
    if (g_flushperiod != -1)
    {
        g_flushcounts++;
        if (g_flushcounts == g_flushperiod)
        {
            #if defined(DEBUG)
                printf("g_flushcounts=%u,empty tlb\n", g_flushcounts);
            #endif
            empty_tlbs(&g_tlbhead);
            g_tlb_node_counts = 0;
            g_flushcounts = 0;
        }
    }
}

void handle_tlb(unsigned int pagenum)
{
    TLB *tmp = search_by_pgnum(g_tlbhead, pagenum);
    g_totalhandled++;
    //hits
    if (tmp != NULL)
    {
        g_totalhits++;
        tmp->miscounts = 0;
        add_all_miscounts(g_tlbhead);
        flush();
        return;
    }
    //miss
    g_totalmisses++;
    if (g_tlb_node_counts == 0) //TLBs are empty
    {
        g_tlbhead = (TLB *)malloc(sizeof(TLB));
        g_tlbhead->pgnum = pagenum;
        g_tlbhead->miscounts = 1;
        g_tlbhead->next = NULL;
        g_tlbtail = g_tlbhead;
        g_fifo_pointer = g_tlbhead;
        g_tlb_node_counts++;
    }
    else if (g_tlb_node_counts != g_tlbsize) //TLBs are not full
    {

        TLB *newnode = (TLB *)malloc(sizeof(TLB));
        newnode->miscounts = 0;
        newnode->pgnum = pagenum;
        newnode->next = NULL;

        g_tlbtail->next = newnode;
        g_tlbtail = newnode;
        g_tlb_node_counts++;
        add_all_miscounts(g_tlbhead);
    }
    else //TLBs are full
    {

        if (g_pflag == 0) //FIFO
        {
            evict(g_fifo_pointer, pagenum);
            if (g_fifo_pointer == g_tlbtail)
            {
                g_fifo_pointer = g_tlbhead;
            }
            else
            {
                g_fifo_pointer = g_fifo_pointer->next;
            }
        }
        else if (g_pflag == 1) //LRU
        {
            TLB *evict_tlb = LRU_search(g_tlbhead);
            evict(evict_tlb, pagenum);
            evict_tlb->miscounts = 0;
        }
        add_all_miscounts(g_tlbhead);
    }
    tlb_traversal(g_tlbhead);
    flush();
    #if defined(DEBUG)
        printf("----------------------------------\n");
    #endif
}