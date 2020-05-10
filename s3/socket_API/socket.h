void creation(int *);
void connection(int ,struct sockaddr_in *, int, char *);
void binding(int ,struct sockaddr_in *, int);
void binding_file(int ,struct sockaddr_in *);
void listening(int );
void sending(char *, int );
void receiving(char *, int );
void receiving_without_printing(char *, int );
void closing(int );
