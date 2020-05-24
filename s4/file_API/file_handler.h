extern sem_t mutex;
extern sem_t mutexFile;
extern int nbClientF;


int get_last_tty();

FILE* new_tty();

//Store the file name in the second char given in argument
void get_file_name(char *, char *);

//Return 1 if the file exist, else return 0
int file_exist(char *);

//Display the files that are stored in the filesToSend directory
//Only after the "/file" command is called
void *thread_files_displaying(void *);

//Parameters : -file's name, -socket of the receiver
//Send the file to the socket of the receiver
void *thread_file_sending(void *);

//Parameters : -file's name, -socket of the receiver , -clientSocket
//Create a thread executing thread_file_sending for each client present in the channel when they succeedingly connect the the newly created socket
void *thread_file_accepting(void *);

//Thread that create a new socket and connect to the one of the client that one to send a file
//Then receives the file and stores it in the dowloadedFile directory
void *thread_file_receiving(void *);
