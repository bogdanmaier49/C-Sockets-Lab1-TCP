#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CLIENTS 50

void error (char* msg) {
	perror (msg);
	exit(1);
}


int numarDeApariti (char* string, int string_length, char c) {
	int i = 0;
	int result = 0;
	for (i = 0; i < string_length; i++)
		if (string[i] == c)
			result ++;

	return result;
}

char* recvString (int client_desc, unsigned int size) {

	char* result = malloc (sizeof(char) * size);

	int i = 0;
	for (i = 0; i < size; i++)
	{
		char c;
		recv(client_desc, &c, sizeof(char), 0);
		c = ntohs(c);
		result[i] = c;
	}

	result[size] = '\0';

	return result;

}

int main () {

	struct sockaddr_in server, client;
	int socket_desc;
	int client_desc;

	socket_desc = socket (AF_INET, SOCK_STREAM, 0);
	if (socket_desc < 0)
		error("Could not create the socket\n");

	memset (&server, 0, sizeof(server));
	server.sin_port = htons(2222);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(socket_desc, (struct sockaddr *)& server, sizeof(server)) < 0)
		error("Could not bind\n");
	
	listen(socket_desc, NUMBER_OF_CLIENTS);

	unsigned int l = sizeof(client);
	memset(&client, 0, sizeof(client));

	while (1) {

		printf("Waiting for a client\n");

		client_desc = accept (socket_desc, (struct sockaddr *) &client, &l);

		printf("A client has connected\n");

		// Primeste marimea stringului

		int size;
		recv(client_desc, &size, sizeof(int), 0);
		size = ntohs(size);
		printf("size: %d\n", size);



		// Primeste cate un caracter din string pe rand

		char string[100];

		recv(client_desc, &string, sizeof(char) * size, 0);
		string[size] = '\0';
		printf("%s\n", string);


		// Primeste caracterul ce trebui cautat 

		char c;
		recv (client_desc, &c, sizeof(char), 0);
		printf("c: %c\n", c);

		// Trimite rezultatul inpoi la clinet

		int rez = numarDeApariti(string, size, c);
		rez = htons(rez);
		send(client_desc, &rez, sizeof(int), 0);

		close(client_desc);

	}

	return 0;
}