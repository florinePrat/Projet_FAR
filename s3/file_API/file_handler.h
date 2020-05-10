extern sem_t mutex;
extern sem_t mutexFile;
extern int nbClient;


int get_last_tty();
FILE* new_tty();
void get_file_name(char *, char *);
int file_exist(char *);

//Display the files that are stored in the filesToSend directory
//Only after the "file" command is called
void *thread_files_displaying(void *);

void *thread_file_sending(void *);


void *thread_file_accepting(void *);

void *thread_file_receiving(void *);
