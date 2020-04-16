#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>


#define MSG 100

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
	//92 is the max number of caracteres
	//it is under 100 so we have enough bytes to store "Client> " on server side
  char *test=fgets(msg,92,stdin);
  if(test==NULL){
    printf("Error while sending your message, please type again\n");
    exit(1);
  }
}

void sending(char *msg, int socket){
  //Sending message
  ssize_t sending=send(socket,msg,100*sizeof(char),0);

  if (sending==-1){
    printf("\nError : sending message\n");
    exit(1);
  }
}

void receiving(char *msg, int socket){
  int receiving=recv(socket, msg,100*sizeof(char), 0);
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
 	char messageSent[MSG];
	char messageReceived[MSG];
  int serverPort=atoi(argv[2]);
  char *ip=argv[1];
  int clientSocket;
  struct sockaddr_in serverAddress;
	pthread_t thread;

  creation(&clientSocket);
  connection(clientSocket,&serverAddress,serverPort,ip);


  receiving(messageReceived,clientSocket);
  receiving(messageReceived,clientSocket);

  //strcmp returns 0 if both strings are the same
  if(!strcmp("Server> Waiting for the other client...\n",messageReceived)){
    receiving(messageReceived,clientSocket);
  }

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
