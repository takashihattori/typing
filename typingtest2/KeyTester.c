#include <stdio.h>

int main(){
  unsigned char code;
  while(code!='q'){
    printf(">>");
    code = getchar();
    printf("%x\n\n",code);
  }
  return 0;
}
