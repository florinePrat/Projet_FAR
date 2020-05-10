#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <regex.h>
#include <dirent.h>
#include <limits.h>
#include <semaphore.h>

#define MSG 200
#define NB_MAX_FILE 100

//Used to wait the server to send us the number of client
sem_t mutex;
int nbClient=0;


sem_t mutexFile;

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

void binding_file(int sock,struct sockaddr_in *addr){
  //Socket binding
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  addr->sin_port = 9000;
  int binding = bind(sock, (struct sockaddr*)addr, sizeof(*addr));

  if(binding==-1){
    printf("Error : socket binding while trying to send a file\n");
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

void typing(char *msg){
  //Typing message
	//89 is the max number of caracteres
	//it is under MSG so we have enough bytes to store the pseudonym on server side
  char *test=fgets(msg,199,stdin);
  if(test==NULL){
    printf("Error while sending your message, please type again\n");
    exit(1);
  }
}

void sending(char *msg, int socket){
  //Sending message
		ssize_t sending=send(socket,msg,MSG*sizeof(char),0);
		if (sending==-1){
			printf("\nError : sending message\n");
			exit(1);
		}
}
void receiving_without_printing(char *msg, int socket){
  int receiving=recv(socket, msg,MSG*sizeof(char), 0);
  if(receiving==-1){
    printf("\nError : receiving message\n");
    exit(1);
  }
}

void receiving(char *msg, int socket){
  receiving_without_printing(msg,socket);
	if(strncmp("file",msg,4) && strncmp("ServerData>",msg,11)){
  	printf(msg);
	}
}

void closing(int socket){
  int closing=close(socket);
  if(closing==-1){
    printf("Error : closing socket\n");
    exit(1);
  }
}




//Check pseudonym
int isValid(char *pseudo){
  regex_t regExp;
    if (strlen(pseudo)==1){
			printf("You can't have an empty pseudonym.\n");
		}else if(strlen(pseudo)>11){
			printf("Your pseudonym must be under 10 characteres.\n");
		}else{
      regcomp(&regExp,"[:alnum:]",0);
      if (regexec(&regExp, pseudo,0,NULL,0) == 0){
          regfree(&regExp);
          return 1;
      }else{
				printf("No special characteres.\n");
        regfree(&regExp);
    	}
    }
		 return 0;
}

int get_last_tty() {
  FILE *fp;
  char path[1035];
  fp = popen("/bin/ls /dev/pts", "r");
  if (fp == NULL) {
    printf("Impossible d'exécuter la commande\n" );
    exit(1);
  }
  int i = INT_MIN;
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    if(strcmp(path,"ptmx")!=0){
      int tty = atoi(path);
      if(tty > i) i = tty;
    }
  }

  pclose(fp);
  return i;
}

FILE* new_tty() {
  pthread_mutex_t the_mutex;
  pthread_mutex_init(&the_mutex,0);
  pthread_mutex_lock(&the_mutex);
  system("gnome-terminal"); sleep(1);
  char *tty_name = ttyname(STDIN_FILENO);
  int ltty = get_last_tty();
  char str[2];
  sprintf(str,"%d",ltty);
  int i;
  for(i = strlen(tty_name)-1; i >= 0; i--) {
    if(tty_name[i] == '/') break;
  }
  tty_name[i+1] = '\0';
  strcat(tty_name,str);
  FILE *fp = fopen(tty_name,"wb+");
  pthread_mutex_unlock(&the_mutex);
  pthread_mutex_destroy(&the_mutex);
  return fp;
}

void get_file_name(char *msg, char *name){
	int pos=0;
	int i;
	for(i=9;i<strlen(msg)-1;i++){
		name[pos]=msg[i];
		pos=pos+1;
	}
	name[pos]='\0';
}

//Store the number of clients connected when the server is sending the corresponding message ("ServerData>..")
//Used to know how many client will need to connect if this client is sending a file
//nbClient<=80
void storeNbClient(char *msg){
	int i;
	char nb[2];
	for(i=11;i<strlen(msg);i++){
		nb[i-11]=msg[i];
	}
	nbClient=atoi(nb);
}

int file_exist(char *msg){
	int i;
	char name[100];
	get_file_name(msg,name);
	char fileTree[1034];
	strcpy(fileTree,"filesToSend/");
	strcat(fileTree,name);
	FILE *fps = fopen(fileTree, "r");
	if (fps == NULL){
		printf("Server> Can't open the following file : %s, files must be stores in the \"filesToSend\" directory\n",fileTree);
		printf("Server> Please type \"file\" to cancel the action or choose a valid file\n");
		return 0;
	}
	fclose(fps);
	return 1;
}

void separate_ip_port(char *msg,char *ip, char *port){
	int i;
	int pos=0;
	char *current=port;
	for(i=5;i<strlen(msg);i++){
		if(msg[i]=='/'){
			current[i]='\0';
			current=ip;
			pos=0;
		}else{
			current[pos]=msg[i];
			pos=pos+1;
		}
	}
	current[pos]='\0';
}

void *thread_file_receiving(void *params){
	char **parameters=(char**)params;
	char ip[20];
	int port=atoi(parameters[1]);
	int fileSocket;
	struct sockaddr_in clientAddr;

	strcpy(ip,parameters[0]);


	creation(&fileSocket);
	connection(fileSocket,&clientAddr,port,ip);
	//Receiving the file's name
	char fileName[100];
	receiving_without_printing(fileName,fileSocket);
	char fileTree[1000]="downloadedFiles/";
	strcat(fileTree,fileName);
	FILE *fps=fopen(fileTree,"w");
	if(fps == NULL){
		printf("Can't create the following file : %s", fileTree);
	}else{
		char fileLine[1000];
		receiving_without_printing(fileLine,fileSocket);
		while(strcmp("file",fileLine)){
			fprintf(fps,"%s\n",fileLine);
			receiving_without_printing(fileLine,fileSocket);
		}
	}
	fclose(fps);
	printf("Server> The file has been downloaded.\n");
	closing(fileSocket);
	pthread_exit(0);
}



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





//Display the files that are stored in the filesToSend directory
//Only after the "file" command is called
void *thread_files_displaying(void *t){
	FILE* fp1 = new_tty();
  fprintf(fp1,"%s\n","This computer terminal will be used for displaying purpose only.");


  DIR *dp;
  struct dirent *ep;
  dp = opendir ("./filesToSend");
  if (dp != NULL) {
    fprintf(fp1,"Here are the files that you can send :\n");
    while (ep = readdir (dp)) {
      if(strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
				fprintf(fp1,"-%s\n",ep->d_name);
    }
    (void) closedir (dp);
  }
  else {
    perror ("Error : you can't open the directory\n");
  }
	printf("Server> To choose a file, enter the command : filename <fileName>\n");
	printf("Server> To cancel the action, enter the command \"file\" again\n");
	pthread_exit(0);
}


void *thread_file_sending(void *params){
	//parameters :
	//-file's name
	//-socket of the receiver
	char **thread_params=(char**)params;
	int sockReceiver=atoi(thread_params[1]);
	sem_post(&mutexFile);
	char fileTree[1034];
	char *name=thread_params[0];
	strcpy(fileTree,"filesToSend/");
	strcat(fileTree,name);

	FILE *fps = fopen(fileTree, "r");
	if (fps == NULL){
		printf("Server> Can't open the following file : %s\n",fileTree);
	}
	else {
		char str[1000];
		//Sending the file's name first
		sending(name,sockReceiver);
		while (fgets(str, 1000, fps) != NULL) {
			str[strlen(str)-1]='\0';
			sending(str,sockReceiver);
		}
		//Ending the file transfer
		sending("file",sockReceiver);
		fclose(fps);
		closing(sockReceiver);

	}
		pthread_exit(0);
}



void *thread_file_accepting(void *params){
	//parameters :
	//-file's name
	//-socket of the receiver (created in the "thread_file_accepting" method)
	//-clientSocket so it can send one message to the server containing data about the new socket
	char **parameters=(char**)params;
	//Creating a socket which sends the file to the server
	int fileSocket;
	struct sockaddr_in address;
	socklen_t addressLength = sizeof(struct sockaddr_in);
	int clientSocket=atoi(parameters[2]);
	creation(&fileSocket);
	binding_file(fileSocket,&address);
	listening(fileSocket);


	//sending data (port used) about the newly created socket
	char data[40];
	sprintf(data,"file/%i",(int) ntohs(address.sin_port));
	sending(data,clientSocket);

	//Waiting for the server to send us the number of clients
	sem_wait(&mutex);
	int nbClientThread=nbClient;
	sem_post(&mutex);
	struct sockaddr_in clientAddress[nbClientThread];
	pthread_t sendthread[nbClientThread];
	int clientReceiverSocket;
	char sockString[10];
	//Accepting connection from server
	int i;
	for(i=0;i<nbClientThread-1;i++){
		clientReceiverSocket=accept((int)fileSocket, (struct sockaddr*) &clientAddress[i], &addressLength);
		if(clientReceiverSocket == -1){
			printf("Error : accepting connection\n");
			exit(1);
		}
		sem_wait(&mutexFile);
		sprintf(sockString,"%i",clientReceiverSocket);
		parameters[1]=sockString;
		pthread_create(&sendthread[i],NULL,thread_file_sending,(void *) parameters);
	}

	for (i=0;i<nbClientThread-1;i++){
	    pthread_join(sendthread[i], NULL);
	}

	closing(fileSocket);
	printf("Server> File sent.\n");
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

	//Message : enter a pseudonym, or warning that the server is full
	receiving(messageReceived,clientSocket);
	if(!strcmp("Server> The server is full.\n",messageReceived)){
		closing(clientSocket);
		exit(1);
	}

	//Enter a pseudonym that must be < 10 caracteres
	do{
		char *test=fgets(messageSent,20,stdin);
		if(test==NULL){
			printf("Error while sending your message, please type again\n");
			exit(1);
		}
	}while(!isValid(messageSent));

	sending(messageSent,clientSocket);

	//Welcoming message
	receiving(messageReceived,clientSocket);


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
	closing(clientSocket);
  return 0;
}
