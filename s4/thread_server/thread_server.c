#include "thread_server.h"
#include "../necessary_librairies.h"
#include "../socket_API/socket.h"
#include "../utility_method/utility_server.h"

void *thread_client(void *params){
  int *thread_param=(int*)params;
  int serverSocket=thread_param[0];
  int channelNum=thread_param[1];
  int clientNum=clientNumTransfer;
  sem_post(&overall_sem);
  pthread_t fthread[NB_MAX_FILES];
  //10 caracteres max for pseudo
  char pseudo[PSEUDO_LENGTH];
  char msg[MSG];
  char msgSent[MSG];
  int i;
  //To register the pseudonym
  sending("Server>Begin",clientSocket[channelNum][clientNum]);
  sprintf(msgSent,"Server> You joined %s, please enter a pseudonym (no more than %i characters, no special characters).\n",channel_name[channelNum],PSEUDO_LENGTH);
  sending(msgSent,clientSocket[channelNum][clientNum]);
  receiving_without_printing(pseudo,clientSocket[channelNum][clientNum]);

  //Welcoming message
  pthread_mutex_lock(&mutexS);
  if(nbClient[channelNum]>1){
    sprintf(msgSent,"Server> Welcome %s, there are %i users on this channel.\n",pseudo,nbClient[channelNum]);
    sending(msgSent,clientSocket[channelNum][clientNum]);
  }else{
    sprintf(msgSent,"Server> Welcome %s, you are alone on this channel.\n",pseudo);
    sending(msgSent,clientSocket[channelNum][clientNum]);
  }
  if(clientSocket[channelNum][clientNum]==channel_admin[channelNum]){
    sending("Server> You are the admin of this channel.\n",clientSocket[channelNum][clientNum]);
    sending("Server> To close this channel, type \"/delete\".\n",clientSocket[channelNum][clientNum]);
  }
  pthread_mutex_unlock(&mutexS);

  //Announcing that someone joined the channel
  if(nbClient[channelNum]>1){
    sprintf(msgSent,"Server> %s joined the channel.\n",pseudo);
    for(i=0;i<NB_MAX_USERS;i++){
      if(!availability[channelNum][i] && i!=clientNum){
        sending(msgSent,clientSocket[channelNum][i]);
      }
    }
  }

  while(1){
    receiving_without_printing(msg,clientSocket[channelNum][clientNum]);
    if(nbClient[channelNum]<2){
      if(!strcmp("/fin\n",msg)) {
        nbClient[channelNum]=nbClient[channelNum]-1;
        availability[channelNum][clientNum]=1;
        availabilityChannel[channelNum]=1;
        nbChannel=nbChannel-1;
        closing(clientSocket[channelNum][clientNum]);
        int tcheck=pthread_cancel(tchannel[channelNum]);
        check_thread(tcheck);
        break;
      }else if(!strcmp("/delete\n",msg)){
        sending("Server> You closed this channel, disconnecting...\n",clientSocket[channelNum][clientNum]);
        nbClient[channelNum]=nbClient[channelNum]-1;
        availability[channelNum][clientNum]=1;
        availabilityChannel[channelNum]=1;
        nbChannel=nbChannel-1;
        closing(clientSocket[channelNum][clientNum]);
        int tcheck=pthread_cancel(tchannel[channelNum]);
        check_thread(tcheck);
        break;
      }else{
        sending("Server> There is noone here, you can't send your message.\n",clientSocket[channelNum][clientNum]);
      }
    }else{
      //If one client sent "fin" then you warn the others
      if(!strcmp("/fin\n",msg)){
        sprintf(msgSent,"Server> %s left the chat.\n",pseudo);
        for(i=0;i<NB_MAX_USERS;i++){
          if(!availability[channelNum][i] && i!=clientNum){
            sending(msgSent,clientSocket[channelNum][i]);
          }
        }
        nbClient[channelNum]=nbClient[channelNum]-1;
        availability[channelNum][clientNum]=1;
        if(clientSocket[channelNum][clientNum]==channel_admin[channelNum]){
          switch_admin(channelNum);
        }
        closing(clientSocket[channelNum][clientNum]);
        break;
      }else if(!strncmp("file/",msg,5)){
        //if "file/port" is received
        //sending the number of clients that will receive the file
        //it is needed because the client will become a server and he needs to know how many clients will connect
        sprintf(msgSent,"ServerData>%i",nbClient[channelNum]);
        sending(msgSent,clientSocket[channelNum][clientNum]);

        //Add the ip to the message and send it to every clients
        sprintf(msgSent,"/%s",inet_ntoa(clientAddress[channelNum][clientNum].sin_addr));
        strcat(msg,msgSent);
        for(i=0;i<NB_MAX_USERS;i++){
          if(!availability[channelNum][i] && i!=clientNum){
            sending(msg,clientSocket[channelNum][i]);
            sprintf(msgSent,"Server> %s is sending a file...\n",pseudo);
            sending(msgSent,clientSocket[channelNum][i]);
          }
        }
      }else if(!strcmp("/delete\n",msg)){
        //Case : deleting the channel
        if(channel_admin[channelNum]!=clientSocket[channelNum][clientNum]){
          sending("Server> You don't have the right to delete this channel.\n",clientSocket[channelNum][clientNum]);
        }else{
          for(i=0;i<NB_MAX_USERS;i++){
            if(!availability[channelNum][i] && i!=clientNum)
              sending("Server> The admin closed this channel, disconnecting...\n",clientSocket[channelNum][i]);
          }
          sending("Server> You closed this channel, disconnecting...\n",clientSocket[channelNum][clientNum]);
          while(nbClient[channelNum]>1){
            sleep(1);
          }
          nbClient[channelNum]=nbClient[channelNum]-1;
          availability[channelNum][clientNum]=1;
          availabilityChannel[channelNum]=1;
          nbChannel=nbChannel-1;
          closing(clientSocket[channelNum][clientNum]);
          int tcheck=pthread_cancel(tchannel[channelNum]);
          check_thread(tcheck);
          break;
        }
      }else{
        //Sending the message

        //Add the pseudonym at the beginning
        strcpy(msgSent,pseudo);
        strcat(msgSent,"> ");
        strcat(msgSent,msg);
        for(i=0;i<NB_MAX_USERS;i++){
          if(!availability[channelNum][i] && i!=clientNum){
            sending(msgSent,clientSocket[channelNum][i]);
          }
        }
      }
    }
    }

  //If the user sent "fin", end the thread
  pthread_exit(0);
}




void *thread_accept(void *t){
  int i;
  int chosenChannel;
  //Had to dynamically allocate the memory because pthread_exit raises a segmentation fault otherwise
  char *msg=(char*)malloc(sizeof(char)*MSG);
  int invalidChannel=1;
  struct taccept_param *param=(struct taccept_param*) t;
  int socket=param->socket;
  struct sockaddr_in address;
  address.sin_family=param->address.sin_family;
  address.sin_port=param->address.sin_port;
  address.sin_addr.s_addr=param->address.sin_addr.s_addr;
  sem_post(&mutex_accept_client);

  while(invalidChannel){
    if(nbChannel==0){
      sending("Server> No channel created, please create a channel with the \"/create\" command\n",socket);
    }else{
      sending("Server> Please enter the number of the channel the channel you want to join :\n",socket);
      for(i=0;i<NB_MAX_CHANNEL;i++){
        if(!availabilityChannel[i]){
          sprintf(msg,"%d. ",i+1);
          sending(msg,socket);
          sending(channel_name[i],socket);
          sending(" | ",socket);
          sending(channel_description[i],socket);
          sprintf(msg," [%i|%i]\n",nbClient[i],NB_MAX_USERS);
          sending(msg,socket);
        }
      }
      sending("or create a new channel with the \"/create\" command\n",socket);
    }
    sending("Server>EndList",socket);
    receiving_without_printing(msg,socket);
    if(!strcmp(msg,"/create")){
      pthread_mutex_lock(&mutex_channel_creation_2);
      if(nbChannel==NB_MAX_CHANNEL){
        sending("Server> The maximum number of channels has been reached, wait for somebody to delete one.\n",socket);
      }else{
        i=0;
        while(i<NB_MAX_CHANNEL && !availabilityChannel[i]){
          i=i+1;
        }
        if(i==NB_MAX_CHANNEL){
          //Should never get here
          perror("Error : creating channel\n");
        }
        receiving_without_printing(channel_name[i],socket);
        receiving_without_printing(channel_description[i],socket);
        channel_admin[i]=socket;
        availabilityChannel[i]=0;
        nbChannel=nbChannel+1;
        sem_wait(&mutex_channelNumTransfer);
        channelNumTransfer=i;
        sem_post(&sem_warning_creation_channel_v2);
      }
      pthread_mutex_unlock(&mutex_channel_creation_2);
      connecting_to_channel(i,&invalidChannel,socket,&address);
    }else if(strcmp(msg,"/resend")){
      connecting_to_channel(atoi(msg)-1,&invalidChannel,socket,&address);
    }
  }
  free(msg);
  pthread_exit(0);
}

void *thread_channel(void* params){
  int *param_thread=(int*)params;
  int tparam[2];
  //server socket
  tparam[0]=param_thread[0];
  //channel number
  tparam[1]=param_thread[1];
  sem_post(&mutex_channel_creation);
  pthread_t tclient[NB_MAX_USERS];
  char msg[100];
  int a;
  while(1){
    sem_wait(&sem_channel[tparam[1]]);
    nbClient[tparam[1]]=nbClient[tparam[1]]+1;
    pthread_create(&tclient[clientNumTransfer],NULL,thread_client,(void*)tparam);
  }
}

void *thread_creation_channel(void *par){
  int *param=(int*)par;
  sem_init(&mutex_channel_creation,0,1);
  sem_init(&mutex_channelNumTransfer,0,1);
  sem_init(&sem_warning_creation_channel_v2,0,0);
  while(1){
    sem_wait(&sem_warning_creation_channel_v2);
    sem_wait(&mutex_channel_creation);
    param[1]=channelNumTransfer;
    sem_post(&mutex_channelNumTransfer);
    int tcheck=pthread_create(&tchannel[param[1]],NULL,thread_channel,(void*)param);
    check_thread(tcheck);
  }
}
