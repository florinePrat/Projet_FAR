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
//Only after the "file" command is called
void *thread_files_displaying(void *);

//Parameters : -file's name, -socket of the receiver
//Send the full file on another thread.
void *thread_file_sending(void *);

//Parameters : -file's name, -socket of the receiver , -clientSocket
void *thread_file_accepting(void *);

//No parameters, 
void *thread_file_receiving(void *);
