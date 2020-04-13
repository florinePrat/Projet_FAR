#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>


#define MSG 100

void creation(int *sock){
  //Socket creation
	*sock=socket(PF_INET,SOCK_STREAM,0);
  if(*sock==-1){
    printf("Error : socket creation");
    exit(1);
  }
}

void connection(int sock,struct sockaddr_in *addr, int port, char *ip){
  //Server connection
  addr->sin_family=AF_INET;
  addr->sin_port=htons(port);
  int addressStruct=inet_pton(AF_INET, ip, &(addr->sin_addr));

  if(addressStruct==0 || addressStruct==-1){
    printf("Error : invalid IP address");
    exit(1);
  }

  socklen_t lengthAddress = sizeof(struct sockaddr_in);

  int connection=connect(sock,(struct sockaddr*)addr, lengthAddress);
  if(connection==-1){
    printf("Error : connection server");
    exit(1);
  }
}

void typing(char *msg){
  printf("You> ");
  //Typing message
  char *test=fgets(msg,100,stdin);
  if(test==NULL){
    printf("Error while sending your message, please type again");
    exit(1);
  }
}

void sending(char *msg, int socket){
  //Sending message
  ssize_t sending=send(socket,msg,100*sizeof(char),0);

  if (sending==-1){
    printf("\nError : sending message");
    exit(1);
  }
}

void receiving(char *msg, int socket){
    printf("Client> ");
  int receiving=recv(socket, msg,100*sizeof(char), 0);
  if(receiving==-1){
    printf("\nError : receiving message");
    exit(1);
  }
  printf(msg);
}

void closing(int socket){
  int closing=close(socket);
  if(closing==-1){
    printf("Error : closing socket");
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

int main(int argc, char *argv[]){
  if(argc<3){
    printf("Error : parameter is missing, server adress and port is required");
    exit(1);
  }

  int serverPort=atoi(argv[2]);
  char *ip=argv[1];
  int clientSocket;
  struct sockaddr_in serverAddress;
  char messageSent[MSG];
  char messageReceived[MSG];


  creation(&clientSocket);
  connection(clientSocket,&serverAddress,serverPort,ip);


  receiving(messageReceived,clientSocket);
  int isSending;
	//strncmp(str1,str2,n)
	//strncmp returns 0 if the n first character of both strings are the same
  if(!strncmp("You joined an ongoing channel",messageReceived,29)){
    isSending=0;
  }else{
    isSending=1;
  }


  receiving(messageReceived,clientSocket);

  //strcmp returns 0 if both strings are the same
  if(!strcmp("Waiting for the other client...\n",messageReceived)){
    receiving(messageReceived,clientSocket);
  }



  while(1){
    if(isSending){
      typing(messageSent);
      sending(messageSent,clientSocket);
			if(!strcmp("fin\n",messageSent)){
				break;
			}
			isSending=0;
    }else{
      receiving(messageReceived,clientSocket);
			if(!strcmp("The other client left the chat, waiting for a client...\n",messageReceived)){
				receiving(messageReceived,clientSocket);
			}
			isSending=1;
    }

  }

  closing(clientSocket);
  return 0;
}
