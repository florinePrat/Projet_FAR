#include "../necessary_librairies.h"
#include "../socket_API/socket.h"
#include "file_handler.h"


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
