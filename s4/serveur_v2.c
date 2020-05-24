#include "necessary_librairies.h"
#include "socket_API/socket.h"
#include "thread_server/thread_server.h"
#include "utility_method/utility_server.h"


//Global variables

//Mutex for nbClient variable
pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
int nbClient[NB_MAX_CHANNEL];
int clientSocket[NB_MAX_CHANNEL][NB_MAX_USERS];
struct sockaddr_in clientAddress[NB_MAX_CHANNEL][NB_MAX_USERS];

//To know which client (his number) is connected
int availability[NB_MAX_CHANNEL][NB_MAX_USERS];
//To know which channel is available
int availabilityChannel[NB_MAX_CHANNEL];

//sem_channel is used to warn a thread that a new client has connected to the channel
//overall_sem ensures that clientNumTransfer is not modified before it is used correctly
sem_t sem_channel[NB_MAX_CHANNEL];
sem_t overall_sem;
sem_t mutex_channelNumTransfer;
int clientNumTransfer;
int channelNumTransfer;
int nbChannel=0;
char channel_name[NB_MAX_CHANNEL][NAME_LENGTH];
char channel_description[NB_MAX_CHANNEL][DESCRIPTION_LENGTH];
//Store the socket of the admin
int channel_admin[NB_MAX_CHANNEL];

//Synchronize the creation of the channels so they don't use the argument sent in the thread before it is extracted
sem_t mutex_channel_creation;
//To avoid that 2 channel are created at the same time and share the same number
pthread_mutex_t mutex_channel_creation_2=PTHREAD_MUTEX_INITIALIZER;
sem_t mutex_accept_client;

//Use to warn the thread when a client wants to create a channel
sem_t sem_warning_creation_channel_v2;

//Store the id of a channel so we can access it when the admin wants to delete it
pthread_t tchannel[NB_MAX_CHANNEL];


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

  pthread_t taccept;
  pthread_t tchannel;

  //Paramaters to send in the thread :
  //-serverSocket
  //-channel number (added un the thread_creation_channel method)
  int thread_parameters[2];

  creation(&serverSocket);
  binding(serverSocket,&address,serverPort);
  listening(serverSocket);
  initiate_array();

  thread_parameters[0]=serverSocket;
  sem_init(&sem_warning_creation_channel_v2,0,0);

  pthread_create(&tchannel,NULL,thread_creation_channel,(void*)thread_parameters);

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
