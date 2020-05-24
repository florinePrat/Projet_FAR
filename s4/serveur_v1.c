#include "necessary_librairies.h"
#include "socket_API/socket.h"
#include "thread_server/thread_server.c"



//Global variables

//Mutex for nbClient variable
pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
int nbClient[NB_MAX_CHANNEL];
int clientSocket[NB_MAX_CHANNEL][NB_MAX_USERS];
struct sockaddr_in clientAddress[NB_MAX_CHANNEL][NB_MAX_USERS];

//To know which client (his number) is connected
int availability[NB_MAX_CHANNEL][NB_MAX_USERS];


//sem_channel is used to warn a thread that a new client has connected to the channel
//overall_sem ensures that clientNumTransfer is not modified before it is used correctly
sem_t sem_channel[NB_MAX_CHANNEL];
sem_t overall_sem;
int clientNumTransfer;
char channel_name[NB_MAX_CHANNEL][NAME_LENGTH];
char channel_description[NB_MAX_CHANNEL][DESCRIPTION_LENGTH];

//Synchronize the creation of every channels so they don't use the argument sent in the thread before it is extracted
sem_t mutex_channel_creation;

sem_t mutex_accept_client;

int main(int argc, char *argv[]){
  if (argc<2){
    printf("Error : parameter is missing, port is required\n");
    exit(1);
  }


  int serverSocket;
  struct sockaddr_in address;
  int serverPort=atoi(argv[1]);

  int temporarySocket;
  struct sockaddr_in temporaryAddress;
  struct taccept_param param;
  socklen_t addressLength = sizeof(struct sockaddr_in);
  int i;
  int j;

  pthread_t taccept;
  pthread_t tchannel[NB_MAX_CHANNEL];

  //Paramaters to send in the thread :
  //-serverSocket
  //-channel number
  int thread_parameters[2];

  creation(&serverSocket);
  binding(serverSocket,&address,serverPort);
  listening(serverSocket);

  thread_parameters[0]=serverSocket;
  sem_init(&mutex_channel_creation,0,1);

  //Initialize the semaphore for each thread per channel and the variable and create the concerned thread
  //Each thread needs to be stopped while no client has connected, that's why sem_channel[i] is initialized to 0
  for(i=0;i<NB_MAX_CHANNEL;i++){
    nbClient[i]=0;
    sem_wait(&mutex_channel_creation);
    thread_parameters[1]=i;
    sprintf(channel_name[i],"Channel %d",i+1);
    sprintf(channel_description[i],"This is the channel %d.",i+1);
    sem_init(&sem_channel[i],0,0);
    pthread_create(&tchannel[i],NULL,thread_channel,(void*)thread_parameters);
    for(j=0;j<NB_MAX_USERS;j++){
      availability[i][j]=1;
    }
  }
  sem_destroy(&mutex_channel_creation);
  sem_init(&mutex_accept_client,0,1);
  sem_init(&overall_sem,0,1);


  while(1){
    //Accepting connection
    temporarySocket=accept((int)serverSocket, (struct sockaddr*) &temporaryAddress, &addressLength);
    if(temporarySocket == -1){
      printf("Error : accepting connection\n");
      exit(1);
    }

    sem_wait(&mutex_accept_client);
    param.socket=temporarySocket;
    param.address.sin_family=temporaryAddress.sin_family;
    param.address.sin_port=temporaryAddress.sin_port;
    param.address.sin_addr.s_addr=temporaryAddress.sin_addr.s_addr;

    pthread_create(&taccept,NULL,thread_accept,(void*)&param);
  }

  return 0;
}
