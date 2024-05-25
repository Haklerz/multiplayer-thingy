#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "events.h"

#define DEFAULT_PORT "6969"
#define BUFFER_SIZE 1024

// Returns the basename of a given path.
const char *get_basename(const char *path) {
	const char *base = strrchr(path, '\\'); // TODO(HL): Make this not be Windows-dependent.
	
	if (!base)
		return path;
	
	return base + 1;
}

// Prints instuctions on how to use the program.
void print_usage(const char *program_path) {
	fprintf(stderr, "Usage:\t%s [-p port]\n", get_basename(program_path));
}

// Tries to create a socket and bind it to a given port.
SOCKET create_bound_socket(const char *port) {
	SOCKET bound_socket = INVALID_SOCKET;

	struct addrinfo *resolved_addresses = NULL;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,		// Allow IPv4 or IPv6
		.ai_socktype = SOCK_DGRAM,	// Datagram socket
		.ai_flags = AI_PASSIVE		// For wildcard IP address
	};

	if (getaddrinfo(NULL, port, &hints, &resolved_addresses) != 0) {
		fprintf(stderr, "Error: Could not resolve a local address and port. WinSock error code: %d\n", WSAGetLastError());
		goto cleanup_addrinfo;
	}

	for (struct addrinfo *address = resolved_addresses; address != NULL; address = address->ai_next) {
		bound_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
		if (bound_socket == INVALID_SOCKET)
			break; // Give up.

		if (bind(bound_socket, address->ai_addr, (int)address->ai_addrlen) != SOCKET_ERROR)
			break; // Success!

		closesocket(bound_socket);
		bound_socket = INVALID_SOCKET;
		// Try next address.
	}

	cleanup_addrinfo:
	freeaddrinfo(resolved_addresses);

	return bound_socket;
}

int socket_address_to_string(struct sockaddr_storage *socket_address, char *ip_address, size_t ip_address_size, uint16_t *port) {
	return 1;
}

int main(int argc, char *argv[]) {
	if (argc <= 0) {
		fprintf(stderr, "Fatal: Program name unavailable.\n");
		return 1;
	}

	// print_usage(argv[0]);

	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0) {
		fprintf(stderr, "Fatal: Could not initialize WinSock. WinSock error code: %d\n", WSAGetLastError());
		return 2;
	}

	SOCKET server_socket = create_bound_socket(DEFAULT_PORT);
	if (server_socket == INVALID_SOCKET) {
		fprintf(stderr, "Fatal: Could not create and bind socket. WinSock error code: %d\n", WSAGetLastError());
		goto cleanup_wsa;
	}

	fprintf(stderr, "Info: Server running on port: %s\n", DEFAULT_PORT);

	for (;;) {
		fprintf(stderr, "Info: Waiting on data from client...\n");

		char data_buffer[BUFFER_SIZE];
		struct sockaddr_storage client_address;
		socklen_t client_address_len = sizeof(client_address);

		int received_bytes = recvfrom(server_socket, data_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &client_address_len);
		if (received_bytes == SOCKET_ERROR) {
			fprintf(stderr, "Fatal: Could not receive data from client. WinSock error code: %d\n", WSAGetLastError());
			goto cleanup_socket;
		}
		data_buffer[received_bytes] = '\0';

		if (data_buffer[0] == 'q') {
			break;
		}

		char client_ip_address[32];
		uint16_t client_port;

		if (client_address.ss_family == AF_INET) {

			socket_address_to_string(&client_address, client_ip_address, sizeof(client_ip_address), &client_port);
		}

		fprintf(stdout, "Debug: Received data from client at %s:%u : \"%s\"\n", client_ip_address, client_port, data_buffer);
	}

	cleanup_socket:
	if (server_socket != INVALID_SOCKET) {
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}

	cleanup_wsa:
	WSACleanup();

	return 0;
}
