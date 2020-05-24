#include "../necessary_librairies.h"
#include "socket.h"

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
