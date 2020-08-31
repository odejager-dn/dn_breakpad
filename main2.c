#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minidump.h"

static char *bla;

int main(int argc, char* argv[]) {
  char **bla_ptr = &bla;
  bla = (char *)malloc(10000);
  strcpy(bla, "Hello, this is a test string to see if a heap object can be indeed traced in the dump file");
  printf("Registering address %p size %d", bla_ptr, (int)sizeof(bla_ptr));
  minidump_register_mem(bla_ptr, sizeof(bla_ptr));
  printf("Registering address %p size %d", bla, 1000);
  minidump_register_mem(bla, 1000);
  create_minidump("/home/dn", 0);
  free(bla);
  bla = NULL;
  return 0;
}

