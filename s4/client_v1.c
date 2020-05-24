#include "necessary_librairies.h"
#include "socket_API/socket.h"
#include "file_API/file_handler.h"
#include "utility_method/utility.h"


//Used to wait the server to send us the number of client
sem_t mutex;
sem_t mutexFile;
int nbClientF=0;


void *thread_receiving(void *socket){
	char messageReceived[MSG];
	int sock=(int)socket;
	pthread_t fthread;

	//Those variables are used to store data of a client who is sending a file
	char ip[20];
	char portString[20];


	int fileSocket;
	char *thread_params[2];

	while(1){
		receiving(messageReceived,sock);

		//If a client is sending us a file
		if(!strncmp(messageReceived,"file/",5)){
			//Split the received message into the corresponding variables
			separate_ip_port(messageReceived,ip,portString);
			thread_params[0]=ip;
			thread_params[1]=portString;
			pthread_create(&fthread,NULL,thread_file_receiving,(void*)thread_params);
		}else if(!strncmp(messageReceived,"ServerData>",11)){
			storeNbClient(messageReceived);
			sem_post(&mutex);
		}
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]){
	if(argc<3){
    printf("Error : parameter is missing, server adress and port is required\n");
    exit(1);
  }
	sem_init(&mutex,0,1);
	sem_init(&mutexFile,0,1);

	int isSendingFile=0;
 	char messageSent[MSG]={'\0'};
	char messageReceived[MSG]={'\0'};
  int serverPort=atoi(argv[2]);
  char *ip=argv[1];
  int clientSocket;
	char clientSocketString[10];
  struct sockaddr_in serverAddress;
	pthread_t thread_receiving_msg;
	pthread_t thread_displaying_file;
	pthread_t thread_sending_file;
  creation(&clientSocket);
  connection(clientSocket,&serverAddress,serverPort,ip);
	sprintf(clientSocketString,"%i",clientSocket);



	//Printing the list of channels and their description
	while(strcmp(messageReceived,"Server>Begin")){
		while(strcmp(messageReceived,"Server>EndList")){
			printf(messageReceived);
			receiving_without_printing(messageReceived,clientSocket);
		}
		receiving_without_printing(messageReceived,clientSocket);
		int nbChannel=atoi(messageReceived);
		//Enter a channel number that must be < nbChannel and > 0
		do{
			typing_with_limit(messageSent,10);
		}while(channelInvalid(nbChannel,messageSent));
		sending(messageSent,socket);
		receiving_without_printing(messageReceived,clientSocket);
	}


	//Create a thread which will take care of receiving messages
	pthread_create(&thread_receiving_msg,NULL,thread_receiving,(void *)clientSocket);
  while(1){
		typing(messageSent);
		if(!strcmp("fin\n",messageSent)){
			sending(messageSent,clientSocket);
			break;
		}else if(!strcmp("file\n",messageSent)){
			if(!isSendingFile){
				isSendingFile=1;
				pthread_create(&thread_displaying_file,NULL,thread_files_displaying,NULL);
			}else{
				isSendingFile=0;
				printf("Server> Action canceled.\n");
			}
		}else if(isSendingFile && !strncmp("filename ",messageSent,9)){
			if(file_exist(messageSent)){
				char fileName[1023];
				//parameters of the thread :
				//-file's name
				//-client socket of the receiver (created in the "thread_file_accepting" method)
				//-clientSocket so it can send one message to the server containing data about the new socket

				get_file_name(messageSent,fileName);
				char *thread_params[3];
				thread_params[0]=fileName;
				thread_params[2]=clientSocketString;

				sem_wait(&mutex);
				int a=pthread_create(&thread_sending_file,NULL,thread_file_accepting,(void *) thread_params);
				if(a!=0){
					printf("error pthread_create\n");
				}
				printf("Server> Sending file...\n");
				isSendingFile=0;
			}
		}else{
			//Case : message to send
			if(strlen(messageSent)==1){
				printf("Server> You can't send an empty message.\n");
			}else if(!strncmp("file/",messageSent,5)){
				printf("Forbidden action : you can't send a message starting with \"file/\".\n");
			}else{
				sending(messageSent,clientSocket);
			}
		}
	}
	sem_destroy(&mutex);
	sem_destroy(&mutexFile);
	closing(clientSocket);
  return 0;
}
