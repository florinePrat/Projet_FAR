extern int availabilityChannel[NB_MAX_CHANNEL];
extern int availability[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int channel_admin[NB_MAX_CHANNEL];
extern int clientSocket[NB_MAX_CHANNEL][NB_MAX_USERS];
extern int clientNumTransfer;


extern char pseudo[PSEUDO_LENGTH];

extern struct sockaddr_in clientAddress[NB_MAX_CHANNEL][NB_MAX_USERS];

extern sem_t overall_sem;

extern sem_t sem_channel[NB_MAX_CHANNEL];

void initiate_array();

void switch_admin(int);

//Method used to factorize the "thread_accept" method
void connecting_to_channel(int, int *, int, struct sockaddr_in *);
