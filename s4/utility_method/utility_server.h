extern int availabilityChannel[NB_MAX_CHANNEL];
extern int availability[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int channel_admin[NB_MAX_CHANNEL];
extern int clientSocket[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int clientNumTransfer;


extern char pseudo[PSEUDO_LENGTH];

extern struct sockaddr_in clientAddress[NB_MAX_CHANNEL][NB_MAX_USERS];

extern sem_t overall_sem;

extern sem_t sem_channel[NB_MAX_CHANNEL];

//Initialize availability[i][j] and availabilityChannel[i] to 1, for all i,j>0, i<NB_MAX_CHANNEL and j<NB_MAX_USERS
void initiate_array();

//Parameter : the number of a channel
//Change the current admin to the first client available
void switch_admin(int);

/**
Parameters :
-the chosen channel by the new client
-the pointer to the boolean variable checking if the chosen channel is valid
-socket of the client
-address of the client
Method used to factorize the "thread_accept" method, allow to leave the while if the chosen channel is valid
**/
void connecting_to_channel(int, int *, int, struct sockaddr_in *);
