#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int create_minidump(char *output_path);

int main(int argc, char* argv[]) {
  char *str = 0, *str2=0;
  str = (char *)malloc(10000);
  strcpy(str, "uioeqwjenm,daiojqnm,nab,nbdqwiojknasm,nbdask");
  //str2[1] = 2;
  create_minidump(".");
  free(str);
  return 0;
}

