//Create a socket and store it in the parameter
//Exit the program if it fails
void creation(int *);

/**
Parameters :
-the socket
-the sockaddr_in struct which stores the data of the server
-port of the server
-ip of the server

Connect to the socket of the server that is concerned by the ip and the port given in the parameters
Exit the program if one of those steps failed
**/
void connection(int ,struct sockaddr_in *, int, char *);

/**
Parameters :
-socket
-the sockaddr_in struct which stores the data of the socket we want to bind
-the port

Bind a socket
Exit the program if it fails
**/
void binding(int ,struct sockaddr_in *, int);

//Same than binding, with no required port (9000 picked)
void binding_file(int ,struct sockaddr_in *);

/**
Parameter : socket

Listen to incoming connection
Exit the program if it fails
**/
void listening(int );

/**
Parameters :
-a message
-socket

Send a message through the socket
Exit the program if it fails
**/
void sending(char *, int );

/**
Parameters :
-char to store the message
-socket

Receive a message through the socket and print it
Exit the program if it fails
**/
void receiving(char *, int );

//Same the receiving except it doesn't print the message
void receiving_without_printing(char *, int );

/**
Parameter : socket

Close the socket
Exit the program if it fails
**/
void closing(int );
