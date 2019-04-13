#include <unistd.h>
#include <string.h>
#include <math.h>
#include "tlb.h"

void parse_valgrind(const char *line, char *ISLM, unsigned long *addr, unsigned int *size);

void flush();

void handle_tlb(unsigned int pagenum);