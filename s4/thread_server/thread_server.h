#include "../necessary_librairies.h"
extern sem_t sem_channel[NB_MAX_CHANNEL];
extern sem_t overall_sem;
extern sem_t mutex_accept_client;
extern sem_t mutex_channel_creation;
extern sem_t mutex_channelNumTransfer;
extern sem_t sem_warning_creation_channel_v2;

extern pthread_mutex_t mutex_channel_creation_2;
extern pthread_mutex_t mutexS;

extern int channelNumTransfer;
extern int clientNumTransfer;
extern int nbChannel;
extern int availability[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int availabilityChannel[NB_MAX_CHANNEL];
extern int nbClient[NB_MAX_CHANNEL];
extern int clientSocket[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int channel_admin[NB_MAX_CHANNEL];

extern char channel_name[NB_MAX_CHANNEL][NAME_LENGTH];
extern char channel_description[NB_MAX_CHANNEL][DESCRIPTION_LENGTH];

extern pthread_t tchannel[NB_MAX_CHANNEL];

extern struct sockaddr_in clientAddress[NB_MAX_CHANNEL][NB_MAX_USERS];

struct taccept_param{
  int socket;
  struct sockaddr_in address;
};

//Parameters : socket of the client
//Hold all the client space, from the beginning to the end of the conversation.
void *thread_client(void*);

//Parameters : server socket and channel number

void *thread_channel(void*);

//Parameters : socket of the client
//Hold the channel space, including creation and joining.
void *thread_accept(void*);

void *thread_creation_channel(void*);
