#include "../necessary_librairies.h"
#include "utility.h"
#include "../socket_API/socket.h"

void typing(char *msg){
  char *test=fgets(msg,MSG,stdin);
  if(test==NULL){
    printf("Error while sending your message\n");
    exit(1);
  }
}

void typing_with_limit(char *msg, int limit){
  char *test=fgets(msg,limit,stdin);
  if(test==NULL){
    printf("Error while sending your message\n");
    exit(1);
  }
}

int channelInvalid(int limit, char* msg){
  int channel=atoi(msg);
  if(channel<1 || channel>limit){
    printf("Not a valid channel\n");
    return 1;
  }else{
    return 0;
  }
}

int channelInvalid_v2(char *msg, int socket){
  char name[NAME_LENGTH];
  char description[DESCRIPTION_LENGTH];
  if(!strncmp("/",msg,1)){
    if(!strcmp(msg,"/create\n")){
      printf("Type \"/cancel\" to cancel the action.\n");
      printf("Enter the name of the channel (2 characters min, %i characters max):\n",NAME_LENGTH);
      do{
        typing_with_limit(name,NAME_LENGTH+7);
        if(!strncmp("/",name,1)){
          if(!strcmp("/cancel\n",name)){
            printf("Action cancelled.\n");
            sending("/resend",socket);
            return 0;
          }else{
            printf("Unknown command.\n");
            strcpy(name," ");
          }
        }
      }while(strlen(name)<=2);
      printf("Enter the description of the channel (2 characters min, %i characters max) :\n",DESCRIPTION_LENGTH);
      do{
        typing_with_limit(description,DESCRIPTION_LENGTH+7);
        if(!strncmp("/",description,1)){
          if(!strcmp("/cancel\n",description)){
            printf("Action cancelled.\n");
            sending("/resend",socket);
            return 0;
          }else{
            printf("Unknown command.\n");
            strcpy(description," ");
          }
        }
      }while(strlen(description)<=2);
      name[strlen(name)-1]='\0';
      description[strlen(description)-1]='\0';
      sending("/create",socket);
      sending(name,socket);
      sending(description,socket);
      return 0;
    }else{
      printf("Unknown command.\n");
      return 1;
    }
  }else{
    int channel=atoi(msg);
    //if not a number, atoi returns 0
    if(channel<1){
      printf("Not a valid channel\n");
      return 1;
    }else{
      sending(msg,socket);
      return 0;
    }
  }
}

int isValid(char *pseudo){
  regex_t regExp;
    if (strlen(pseudo)==0){
			printf("You can't have an empty pseudonym.\n");
    }else if(!strcmp(pseudo,"file") || !strcmp(pseudo,"Server") || !strcmp(pseudo,"ServerData")) {
      printf("Forbidden pseudonym.\n");
		}else if(strlen(pseudo)>PSEUDO_LENGTH+1){
			printf("Your pseudonym must be under %i characters.\n",PSEUDO_LENGTH);
		}else{
      int test=regcomp(&regExp,"^[a-zA-Z0-9]+$",REG_EXTENDED);
      if(test!=0){
        perror("Error: regexp\n");
        exit(1);
      }
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
	nbClientF=atoi(nb);
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
