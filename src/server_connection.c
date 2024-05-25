#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "events.h"

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "6969"

#define BUFFER_SIZE 1024

// Tries to resolve a server address and create a socket for communicating with the server.
SOCKET create_client_socket(const char *host, const char *port, struct sockaddr_storage *server_address) {
	SOCKET client_socket = INVALID_SOCKET;

	struct addrinfo *resolved_addresses = NULL;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,		// Allow IPv4 or IPv6
		.ai_socktype = SOCK_DGRAM	// Datagram socket
	};

	if (getaddrinfo(host, port, &hints, &resolved_addresses) != 0) {
		fprintf(stderr, "Error: Could not resolve a server address and port. WinSock error code: %d\n", WSAGetLastError());
		goto cleanup_addrinfo;
	}

	for (struct addrinfo *address = resolved_addresses; address != NULL; address = address->ai_next) {
		client_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
		if (client_socket != INVALID_SOCKET) {
			// Success!
			memcpy(server_address, address->ai_addr, address->ai_addrlen);
			break;
		}
		
		// Try next address.
	}

	cleanup_addrinfo:
	freeaddrinfo(resolved_addresses);

	return client_socket;
}

int main(void) {
	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		fprintf(stderr, "Fatal: Could not initialize WinSock. WinSock error code: %d\n", WSAGetLastError());
		return 1;
	}

	struct sockaddr_storage server_address;
	socklen_t server_address_len = sizeof(server_address);

	SOCKET client_socket = create_client_socket(DEFAULT_HOST, DEFAULT_PORT, &server_address);
	if (client_socket == INVALID_SOCKET) {
		fprintf(stderr, "Fatal: Could not create socket. WinSock error code: %d\n", WSAGetLastError());
		goto cleanup_wsa;
	}

	fprintf(stderr, "Info: Connecting to server at %s:%s\n", DEFAULT_HOST, DEFAULT_PORT);

	char data_buffer[BUFFER_SIZE];

	for (;;) {
		fgets(data_buffer, BUFFER_SIZE, stdin);
		data_buffer[strcspn(data_buffer, "\r\n")] = '\0';

		int bytes_sent = sendto(client_socket, data_buffer, strlen(data_buffer), 0, (struct sockaddr *)&server_address, server_address_len);
		if (bytes_sent == SOCKET_ERROR) {
			fprintf(stderr, "Fatal: Could not send data to server. WinSock error code: %d\n", WSAGetLastError());
			goto cleanup_socket;
		}

		if (data_buffer[0] == 'q') {
			break;
		}
	}

	cleanup_socket:
	if (client_socket != INVALID_SOCKET) {
		closesocket(client_socket);
		client_socket = INVALID_SOCKET;
	}

	cleanup_wsa:
	WSACleanup();

	return 0;
}