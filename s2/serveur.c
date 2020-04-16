#define _POSIX_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>


#define MSG 100

//Mutex for nbClient variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int nbClient=0;
int clientSocket[2];
//if availableNum==1, then the next client to join will be the client 1, else it will be the client 2
int availableNum=1;

void creation(int *sock){
  //Socket creation
  *sock=socket(PF_INET, SOCK_STREAM, 0);
  if(*sock == -1) {
    printf("Error : socket creation\n");
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
    printf("Error : socket binding (you may need to wait)\n");
    exit(1);
  }
}

void listening(int sock){
  //Listening to incoming connection
  int listening=listen(sock,1);

  if(listening==-1){
    printf("Error : listen to incoming connection\n");
    exit(1);
  }
}


void receiving(char *msg, int sock){
  int receiving=recv(sock, msg,100*sizeof(char), 0);
  if(receiving==-1){
    printf("Error : receiving message\n");
    exit(1);
  }
}

void sending(char *msg, int sock){
  int sending=send(sock,msg,100*sizeof(char),0);
  if(sending==-1){
    printf("Error : sending message\n");
    exit(1);
  }
}

void closing(int socket){
  int closing=close(socket);
  if(closing==-1){
    printf("Error : closing socket\n");
    exit(1);
  }
}




void *thread_client(void *serverSocket){
  //client number :
  //clientNum==0 : client 1
  //clientNum==1 : client 2
  int clientNum;
  //client number 2 (the one we're communicating with)
  //same than client number
  int clientNumCo;

  //To know if the thread need to accept a new connection
  int lackingClient=1;

  struct sockaddr_in clientAddress;
  char msg[MSG];
  char msgSent[MSG];

  while(1){
    if(lackingClient){
      if(availableNum){
        clientNum=0;
        clientNumCo=1;
        availableNum=0;
      }else{
        clientNum=1;
        clientNumCo=0;
      }

      //Accepting connection
      socklen_t addressLength = sizeof(struct sockaddr_in);
      clientSocket[clientNum]=accept((int)serverSocket, (struct sockaddr*) &clientAddress, &addressLength);
      if(clientSocket[clientNum] == -1){
        printf("Error : accepting connection\n");
        exit(1);
      }
      char firstMsg[65];
      //Mainly to distinguish if one client joined a new conversation or an old one
      //but it also sends the same message to the second client who joins the conversation
      pthread_mutex_lock(&mutex);
      if(nbClient==0){
        sprintf(firstMsg,"Server> You are the client %i.\n",clientNum+1);
        sending(firstMsg,clientSocket[clientNum]);
      }else{
        sprintf(firstMsg,"Server> You joined an ongoing channel, you are the client %i.\n",clientNum+1);
        sending(firstMsg,clientSocket[clientNum]);
      }

      lackingClient=0;
      nbClient=nbClient+1;

      //Put the first client to join the communication on hold
      if(nbClient<2){
        sending("Server> Waiting for the other client...\n",clientSocket[clientNum]);
      }else{
        //Communication starts
        sending("Server> You can now communicate\n\n",clientSocket[clientNum]);
        sending("Server> You can now communicate\n\n",clientSocket[clientNumCo]);
      }
      pthread_mutex_unlock(&mutex);

    }else{


      receiving(msg,clientSocket[clientNum]);
      pthread_mutex_lock(&mutex);
      if(nbClient<2){
        sending("Server> There is noone here, you can't send your message.\n",clientSocket[clientNum]);
      }else{
        //If one client sent "fin" then you warn the other client about it and you put him on hold
        if(!strcmp("fin\n",msg)){
          sending("Server> The other client left the chat, waiting for a client...\n",clientSocket[clientNumCo]);
          nbClient=nbClient-1;
          lackingClient=1;

          //if the client 1 ended the communication, the next client will be the client 1
          if(!clientNum){
            availableNum=1;
          }
          closing(clientSocket[clientNum]);
        }else{
          //Add "Client> at the beginning"
          strcpy(msgSent,"Client> ");
          strcat(msgSent,msg);
          sending(msgSent,clientSocket[clientNumCo]);
        }
      }
      pthread_mutex_unlock(&mutex);
    }
  }
}





int main(int argc, char *argv[]){
  if (argc<2){
    printf("Error : parameter is missing, port is required\n");
    exit(1);
  }

  int serverSocket;
  struct sockaddr_in address;
  int serverPort=atoi(argv[1]);

  creation(&serverSocket);
  binding(serverSocket,&address,serverPort);
  listening(serverSocket);

  int i;
	pthread_t thread[2];
  for (i=0;i<2;i++)
    pthread_create(thread+i,NULL,thread_client,(void *)serverSocket);

  for (i=0;i<2;i++)
    pthread_join(thread[i], NULL);

  return 0;
}
