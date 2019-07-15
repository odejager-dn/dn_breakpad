// segfaulty.c

#include <stdio.h>
#include <stdlib.h>


#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

void handler(int sig) {
    printf("Error: signal %d:\n", sig);
    printf("I can do whatever I want in here!\n");
    exit(1);
}


int main() {
    printf("it goes downhill from here ...\n");
    signal(SIGSEGV, handler);

    int i = *(int*)0;
    return 0;
}

