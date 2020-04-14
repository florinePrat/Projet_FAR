CC=gcc

# The port used by the server
PORT = 8080

# Server's IP
IP_SERVER = 127.0.0.1

# Compile serveur.c
serveur: serveur.c
	$(CC) -o $@ $<

# Compile client.c
client: client.c
	$(CC) -o $@ $<


# Run the server
run_serveur: serveur
	./serveur ${PORT}

# Run the client 	
run_client:
	./client ${IP_SERVER} ${PORT}

clean:
	rm -f serveur client
