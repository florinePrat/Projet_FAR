#include "../necessary_librairies.h"
#include "utility_server.h"
#include "../socket_API/socket.h"

void initiate_array(){
  int i;
  int j;
  for(i=0;i<NB_MAX_CHANNEL;i++){
    availabilityChannel[i]=1;
    for(j=0;j<NB_MAX_USERS;j++){
      availability[i][j]=1;
    }
    printf("\n");
  }
}

void switch_admin(int channel){
  int i=0;
  int j;
  char msg[MSG];
  while(i<NB_MAX_USERS && availability[channel][i]){
    printf("%i\n",availability[channel][i]);
    i=i+1;
  }
  if(i==NB_MAX_USERS){
    //should never get here
    perror("Error : switching admin.\n");
    exit(1);
  }
  channel_admin[channel]=clientSocket[channel][i];
  sending("Server> You have become the admin of this channel.\n",channel_admin[channel]);
  sending("Server> To close this channel, type \"/delete\".\n",channel_admin[channel]);
}

void connecting_to_channel(int chosenChannel, int *validity, int socket, struct sockaddr_in *address){
  int i=0;
  if(availabilityChannel[chosenChannel]){
    sending("Server> This channel doesn't exit.\n",socket);
  }else{
    while(i<NB_MAX_USERS && !availability[chosenChannel][i]){
      i=i+1;
    }
    if(i==NB_MAX_USERS){
      sending("Server> This channel is full.\n",socket);
    }else{
      availability[chosenChannel][i]=0;
      clientSocket[chosenChannel][i]=socket;
      clientAddress[chosenChannel][i].sin_family=address->sin_family;
      clientAddress[chosenChannel][i].sin_port=address->sin_port;
      clientAddress[chosenChannel][i].sin_addr.s_addr=address->sin_addr.s_addr;
      sem_wait(&overall_sem);
      clientNumTransfer=i;
      sem_post(&sem_channel[chosenChannel]);
      *validity=0;
    }
  }
}
