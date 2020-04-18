#define _POSIX_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>


#define MSG 200
#define NB_MAX_USERS 80


//Global variables

//Mutex for nbClient variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int nbClient=0;
int clientSocket[NB_MAX_USERS];

//To know which client (his number) is connected
int availability[NB_MAX_USERS];


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
  int receiving=recv(sock, msg,MSG*sizeof(char), 0);
  if(receiving==-1){
    printf("Error : receiving message\n");
    exit(1);
  }
}

void sending(char *msg, int sock){
  int sending=send(sock,msg,MSG*sizeof(char),0);
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


void *thread_client(void *params){
  int *thread_param=(int*)params;
  int serverSocket=thread_param[0];
  int clientNum=thread_param[1];

  //10 caracteres max for pseudo
  char pseudo[10];
  char msg[MSG];
  char msgSent[MSG];
  int i;

  //To register the pseudonym
  sprintf(msgSent,"Server> You joined a channel, please enter a pseudonym (no more than 10 characteres, no special characteres).\n");
  sending(msgSent,clientSocket[clientNum]);
  receiving(pseudo,clientSocket[clientNum]);
  pseudo[strlen(pseudo)-1]='\0';

  //Welcoming message
  pthread_mutex_lock(&mutex);
  if(nbClient>1){
    sprintf(msgSent,"Server> Welcome %s, there are %i users on this channel.\n",pseudo,nbClient);
    sending(msgSent,clientSocket[clientNum]);
  }else{
    sprintf(msgSent,"Server> Welcome %s, you are alone on this channel.\n",pseudo);
    sending(msgSent,clientSocket[clientNum]);
  }
  pthread_mutex_unlock(&mutex);

  //Announcing that someone joined the channel
  if(nbClient>1){
    sprintf(msgSent,"Server> %s joined the channel.\n",pseudo);
    for(i=0;i<80;i++){
      if(!availability[i] && i!=clientNum){
        sending(msgSent,clientSocket[i]);
      }
    }
  }

  while(1){
    receiving(msg,clientSocket[clientNum]);
    pthread_mutex_lock(&mutex);
    if(nbClient<2){
      sending("Server> There is noone here, you can't send your message.\n",clientSocket[clientNum]);
      if(!strcmp("fin\n",msg)){
        nbClient=nbClient-1;
        availability[clientNum]=1;
        closing(clientSocket[clientNum]);
        break;
      }
    }else{
      //If one client sent "fin" then you warn the others
      if(!strcmp("fin\n",msg)){
        sprintf(msgSent,"Server> %s left the chat.\n",pseudo);
        for(i=0;i<80;i++){
          if(!availability[i] && i!=clientNum){
            sending(msgSent,clientSocket[i]);
          }
        }
        nbClient=nbClient-1;
        availability[clientNum]=1;
        closing(clientSocket[clientNum]);
        break;
      }else{
        //Sending the message

        //Add the pseudonym at the beginning
        strcpy(msgSent,pseudo);
        strcat(msgSent,"> ");
        strcat(msgSent,msg);
        for(i=0;i<80;i++){
          if(!availability[i] && i!=clientNum){
            sending(msgSent,clientSocket[i]);
          }
        }
      }
    }

      pthread_mutex_unlock(&mutex);
    }

    //Unlock the mutex in case the user sent "fin"
    pthread_mutex_unlock(&mutex);
    //If the user sent "fin", end the thread
    pthread_exit(0);
  }





int main(int argc, char *argv[]){
  if (argc<2){
    printf("Error : parameter is missing, port is required\n");
    exit(1);
  }

  //Initialising availability[NB_MAX_USERS]
  int j;
  for(j=0;j<NB_MAX_USERS;j++){
    availability[j]=1;
  }

  int serverSocket;
  struct sockaddr_in address;
  int serverPort=atoi(argv[1]);

  creation(&serverSocket);
  binding(serverSocket,&address,serverPort);
  listening(serverSocket);


  int temporarySocket;
  socklen_t addressLength = sizeof(struct sockaddr_in);
  int i;

  pthread_t thread[NB_MAX_USERS];
  struct sockaddr_in clientAddress;

  //Paramaters to send in the thread :
  //-serverSocket
  //-position of the client is the clientSocket array
  int thread_parameters[2];
  thread_parameters[0]=serverSocket;

  while(1){
    //Accepting connection
    temporarySocket=accept((int)serverSocket, (struct sockaddr*) &clientAddress, &addressLength);
    if(temporarySocket == -1){
      printf("Error : accepting connection\n");
      exit(1);
    }
    i=0;
    //if availability[i] then there is a no connected client in this position
    //we increment until we find an available position
    while(i<80 && !availability[i]){
      i=i+1;
    }
    pthread_mutex_lock(&mutex);
    if(i<80){
      //Create a thread that handles receiving and sending messages
      nbClient=nbClient+1;
      availability[i]=0;
      thread_parameters[1]=i;
      clientSocket[i]=temporarySocket;
      pthread_create(thread+i,NULL,thread_client,(void *)thread_parameters);
    }else{
      //If the server is full
      sending("Server> The server is full.\n",temporarySocket);
    }
    pthread_mutex_unlock(&mutex);
  }
  return 0;
}
