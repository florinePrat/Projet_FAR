CC=gcc

# The port used by the server
PORT = 9005

# Server's IP
IP_SERVER = 127.0.0.1

# Compile serveur.c
serveur: serveur.c
	$(CC) -o $@ $< -lpthread

# Compile client.c
client: client.c
	$(CC) -o $@ $< -lpthread


# Run the server
run_serveur: serveur
	./serveur ${PORT}

# Run the client 	
run_client:
	./client ${IP_SERVER} ${PORT}

clean:
	rm -f serveur client
