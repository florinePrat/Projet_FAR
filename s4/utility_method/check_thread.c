#include "../necessary_librairies.h"

void check_thread(int a){
  if(a!=0){
    perror("Error : creating thread\n");
    exit(1);
  }
}
