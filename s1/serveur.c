#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MSG 100

void creation(int *sock){
  //Socket creation
  *sock=socket(PF_INET, SOCK_STREAM, 0);
  if(*sock == -1) {
    printf("Error : socket creation");
    exit(1);
  }
}

void binding(int sock,struct sockaddr_in *addr, int port){
  //Socket binding
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_port = htons(port);
  int binding = bind(sock, (struct sockaddr*)addr, sizeof(*addr));

  if(binding==-1){
    printf("Error : socket binding");
    exit(1);
  }
}

void listening(int sock){
  //Listening to incoming connection
  int listening=listen(sock,1);

  if(listening==-1){
    printf("Error : listen to incoming connection");
    exit(1);
  }
}


void receiving(char *msg, int sock){
  int receiving=recv(sock, msg,100*sizeof(char), 0);
  if(receiving==-1){
    printf("Error : receiving message");
    exit(1);
  }
}

void sending(char *msg, int sock){
  int sending=send(sock,msg,100*sizeof(char),0);
  if(sending==-1){
    printf("Error : sending message");
    exit(1);
  }
}

void closing(int socket){
  int closing=close(socket);
  if(closing==-1){
    printf("Error : closing socket");
    exit(1);
  }
}

void switchCurrent(int *a,int *b){
  if(*a==0){
    *a=1;
    *b=0;
  }else{
    *a=0;
    *b=1;
  }
}

int main(int argc, char *argv[]){
  if (argc<2){
    printf("Error : parameter is missing, port is required");
    exit(1);
  }

  int serverSocket;
  struct sockaddr_in address;
  int serverPort=atoi(argv[1]);

  creation(&serverSocket);
  binding(serverSocket,&address,serverPort);
  listening(serverSocket);


  int nbClient=0;
  int clientSocket[2];
  char msg[MSG];
  struct sockaddr_in clientAddress[2];
  int currentSender=0;
  int currentReceiver=1;

  while(1){
    if(nbClient<2){
      //Accepting connection
      socklen_t addressLength = sizeof(struct sockaddr_in);
      clientSocket[currentSender]=accept(serverSocket, (struct sockaddr*) clientAddress+currentSender, &addressLength);
      if(clientSocket[currentSender] == -1){
        printf("Error : accepting connection");
        exit(1);
      }
      char firstMsg[65];
      //Mainly to distinguish if one client joined a new conversation or an old one
      //but it also sends the same message to the second client who joins the conversation
      if(nbClient==0){
        sprintf(firstMsg,"You are the client %i.\n",currentSender+1);
        sending(firstMsg,clientSocket[currentSender]);
      }else{
        sprintf(firstMsg,"You joined an ongoing channel, you are the client %i.\n",currentSender+1);
        sending(firstMsg,clientSocket[currentSender]);
      }


      nbClient=nbClient+1;
      //Put the first client to join the communication on hold
      if(nbClient<2){
        sending("Waiting for the other client...\n",clientSocket[currentSender]);
        switchCurrent(&currentSender,&currentReceiver);
      }else{
        //Communication starts
        sending("You can now communicate\n",clientSocket[currentSender]);
        sending("You can now communicate\n",clientSocket[currentReceiver]);
        switchCurrent(&currentSender,&currentReceiver);
      }
    }else{
      receiving(msg,clientSocket[currentSender]);
      //If one client sent "fin" then you warn the other client about it and you put him on hold
      if(!strcmp("fin\n",msg)){
        sending("The other client left the chat, waiting for a client...\n",clientSocket[currentReceiver]);
        nbClient=nbClient-1;
        closing(clientSocket[currentSender]);
      }else{
        sending(msg,clientSocket[currentReceiver]);
        switchCurrent(&currentSender,&currentReceiver);
      }
    }
  }

  return 0;
}
