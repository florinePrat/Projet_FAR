extern int nbClientF;


/**
Parameter : the string used to store the message
Store the message typed by the user
**/
void typing(char *);

/**
Same than typing, but with a limited number of character in the parameter
**/
void typing_with_limit(char*,int);


/**
Parameters :
-the higher channel that the client can join
-the chosen chosen in a string format

Returns 1 if chosen channel is between 1 and the limit, else 0
**/
int channelInvalid(int, char*);

/**
Parameters :
-the chosen chosen in a string format
-the socket of the client

Send to the server the chosen channel if it is a number over 0, or do the necessary to create a channel and send it to the server
If everything above succeeds, returns 0, else 1

**/
int channelInvalid_v2(char*,int);

//Check pseudonym in parameter is valid
int isValid(char *);

//Store the number of clients connected when the server is sending the corresponding message ("ServerData>..")
//Used to know how many client will need to connect if this client is sending a file
//nbClient<=80
void storeNbClient(char *);

/**
Parameter :
-message in the "file/port/ip" format
-char array to store the ip
-char array to store the port
Separate the initial message and store the part in the concerned char array
**/
void separate_ip_port(char *,char *, char *);
