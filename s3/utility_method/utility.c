#include "../necessary_librairies.h"
#include "utility.h"

void typing(char *msg){
  char *test=fgets(msg,199,stdin);
  if(test==NULL){
    printf("Error while sending your message, please type again\n");
    exit(1);
  }
}


int isValid(char *pseudo){
  regex_t regExp;
    if (strlen(pseudo)==1){
			printf("You can't have an empty pseudonym.\n");
		}else if(strlen(pseudo)>11){
			printf("Your pseudonym must be under 10 characteres.\n");
		}else{
      regcomp(&regExp,"[:alnum:]",0);
      if (regexec(&regExp, pseudo,0,NULL,0) == 0){
          regfree(&regExp);
          return 1;
      }else{
				printf("No special characteres.\n");
        regfree(&regExp);
    	}
    }
		 return 0;
}


void storeNbClient(char *msg){
	int i;
	char nb[2];
	for(i=11;i<strlen(msg);i++){
		nb[i-11]=msg[i];
	}
	nbClient=atoi(nb);
}


void separate_ip_port(char *msg,char *ip, char *port){
	int i;
	int pos=0;
	char *current=port;
	for(i=5;i<strlen(msg);i++){
		if(msg[i]=='/'){
			current[i]='\0';
			current=ip;
			pos=0;
		}else{
			current[pos]=msg[i];
			pos=pos+1;
		}
	}
	current[pos]='\0';
}
