CC=gcc

# The port used by the server
PORT = 9009

# Server's IP
IP_SERVER = 127.0.0.1

# Compile serveur_v1.c
serveur: serveur_v1.c socket_API/socket.c thread_server/thread_server.c utility_method/check_thread.c
	$(CC) -o $@ $^ -lpthread

# Compile client_v1.c
client: client_v1.c socket_API/socket.c file_API/file_handler.c utility_method/utility.c utility_method/check_thread.c
	$(CC) -o $@ $^ -lpthread

# Compile client_v2.c
client_v2: client_v2.c socket_API/socket.c file_API/file_handler.c utility_method/utility.c utility_method/check_thread.c
	$(CC) -o $@ $^ -lpthread

 # Compile serveur_v2.c
serveur_v2: serveur_v2.c socket_API/socket.c thread_server/thread_server.c utility_method/check_thread.c utility_method/utility_server.c
	$(CC) -o $@ $^ -lpthread


# Run the server
run_serveur: serveur
	./serveur ${PORT}

# Run the client
run_client:
	./client ${IP_SERVER} ${PORT}

# Run the serveur v2
run_serveur_v2: serveur_v2
	./serveur_v2 ${PORT}

# Run the client v2
run_client_v2: client_v2
	./client_v2 ${IP_SERVER} ${PORT}


clean:
	rm -f serveur client serveur_v2 client_v2
