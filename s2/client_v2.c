#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <regex.h>

#define MSG 200

//global variable wich warns the mains thread when the communication is over
int fin=0;

void creation(int *sock){
  //Socket creation
	*sock=socket(PF_INET,SOCK_STREAM,0);
  if(*sock==-1){
    printf("Error : socket creation\n");
    exit(1);
  }
}

void connection(int sock,struct sockaddr_in *addr, int port, char *ip){
  //Server connection
  addr->sin_family=AF_INET;
  addr->sin_port=htons(port);
  int addressStruct=inet_pton(AF_INET, ip, &(addr->sin_addr));

  if(addressStruct==0 || addressStruct==-1){
    printf("Error : invalid IP address\n");
    exit(1);
  }

  socklen_t lengthAddress = sizeof(struct sockaddr_in);

  int connection=connect(sock,(struct sockaddr*)addr, lengthAddress);
  if(connection==-1){
    printf("Error : connection server\n");
    exit(1);
  }
}

void typing(char *msg){
  //Typing message
	//89 is the max number of caracteres
	//it is under MSG so we have enough bytes to store the pseudonym on server side
  char *test=fgets(msg,199,stdin);
  if(test==NULL){
    printf("Error while sending your message, please type again\n");
    exit(1);
  }
}

void sending(char *msg, int socket){
  //Sending message
  ssize_t sending=send(socket,msg,MSG*sizeof(char),0);

  if (sending==-1){
    printf("\nError : sending message\n");
    exit(1);
  }
}

void receiving(char *msg, int socket){
  int receiving=recv(socket, msg,MSG*sizeof(char), 0);
  if(receiving==-1){
    printf("\nError : receiving message\n");
    exit(1);
  }
  printf(msg);
}

void closing(int socket){
  int closing=close(socket);
  if(closing==-1){
    printf("Error : closing socket\n");
    exit(1);
  }
}

int endOfChat(char *msg){
	if(msg[0]=='f' && msg[1]=='i' && msg[2]=='n' && msg[3]=='\n'){
		return 1;
	}else{
		return 0;
	}
}


//Check pseudonym
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



void *thread_receiving(void *socket){
	char messageReceived[MSG];
	while(1){
		receiving(messageReceived,(int)socket);

	}
	pthread_exit(0);
}

int main(int argc, char *argv[]){
  if(argc<3){
    printf("Error : parameter is missing, server adress and port is required\n");
    exit(1);
  }
 	char messageSent[MSG]={'\0'};
	char messageReceived[MSG]={'\0'};
  int serverPort=atoi(argv[2]);
  char *ip=argv[1];
  int clientSocket;
  struct sockaddr_in serverAddress;
	pthread_t thread;

  creation(&clientSocket);
  connection(clientSocket,&serverAddress,serverPort,ip);


	//Message : enter a pseudonym, or warning that the server is full
	receiving(messageReceived,clientSocket);
	if(!strcmp("Server> The server is full.\n",messageReceived)){
		closing(clientSocket);
		exit(1);
	}

	//Enter a pseudonym that must be < 10 caracteres
	do{
		char *test=fgets(messageSent,20,stdin);
		if(test==NULL){
			printf("Error while sending your message, please type again\n");
			exit(1);
		}
	}while(!isValid(messageSent));

	sending(messageSent,clientSocket);

	//Welcoming message
	receiving(messageReceived,clientSocket);


	//Create a thread which will take care of receiving messages
	pthread_create(&thread,NULL,thread_receiving,(void *)clientSocket);

  while(1){
		typing(messageSent);
		sending(messageSent,clientSocket);
		if(endOfChat(messageSent)){
			break;
		}
	}
	closing(clientSocket);
  return 0;
}
