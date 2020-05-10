extern int nbClient;


//Typing message
//89 is the max number of caracteres
//it is under MSG so we have enough bytes to store the pseudonym on server side
void typing(char *);

//Check pseudonym
int isValid(char *);

//Store the number of clients connected when the server is sending the corresponding message ("ServerData>..")
//Used to know how many client will need to connect if this client is sending a file
//nbClient<=80
void storeNbClient(char *);


void separate_ip_port(char *,char *ip, char *);
