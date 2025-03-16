
// =======================================First Version========================================//

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>

// #define TARGET_IP "192.168.1.1"
// #define START_PORT 1
// #define END_PORT 600


// int main(void) {


// 	int sock;
// 	struct sockaddr_in target;
// 	int port;

// 	memset(&target, 0, sizeof(target));
// 	target.sin_family = AF_INET;

// 	if (inet_pton(AF_INET, TARGET_IP, &target.sin_addr) <= 0) {
// 		printf("[-] Invalied Ip Address.\n");
// 		return 1;
// 	}

// 	printf("[+] Port Scanning On %s\n", TARGET_IP);

// 	for (port = START_PORT; port < END_PORT; port++) {
// 		sock = socket(AF_INET, SOCK_STREAM, 0);
// 		if (sock < 0) {
// 			return 1;
// 		}
// 	}

// 	target.sin_port = htons(port);
// 	if (connect(sock, (struct sockaddr*)&target, sizeof(target)) == 0) {
// 		printf("Port %d Is Open.\n", port);
// 	}

// 	close(sock);

// 	printf("[#] Scanning Complete.\n");
// 	printf("[!] Press <ENTER> to Quit  ");
// 	getchar();
// 	return 0;
// }


// =======================================Second Version========================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DEFAULT_TARGET_IP "192.168.1.1"
#define DEFAULT_START_PORT 1
#define DEFAULT_END_PORT 1000

int main(void) {
    char target_ip[32];
    int start_port, end_port;
    int sock, port, open_ports = 0;
    struct sockaddr_in target;
    struct timeval timeout;
    int flags, error;
    socklen_t len = sizeof(error);
    printf("Enter target IP (Default: %s): ", DEFAULT_TARGET_IP);
    fgets(target_ip, sizeof(target_ip), stdin);
    target_ip[strcspn(target_ip, "\n")] = 0;
    if (strlen(target_ip) == 0) strcpy(target_ip, DEFAULT_TARGET_IP);

    printf("Enter start port (Default: %d): ", DEFAULT_START_PORT);
    if (scanf("%d", &start_port) != 1) start_port = DEFAULT_START_PORT;

    printf("Enter end port (Default: %d): ", DEFAULT_END_PORT);
    if (scanf("%d", &end_port) != 1) end_port = DEFAULT_END_PORT;

    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;

    if (inet_pton(AF_INET, target_ip, &target.sin_addr) <= 0) {
        printf("[-] Invalid IP Address.\n");
        return 1;
    }

    printf("\n[+] Scanning target: %s from port %d to %d\n====================================================\n", target_ip, start_port, end_port);

    for (port = start_port; port <= end_port; port++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("[-] Socket creation failed");
            continue;  
        }
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        target.sin_port = htons(port);
        connect(sock, (struct sockaddr*)&target, sizeof(target));
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        struct timeval select_timeout = {1, 0}; 

        if (select(sock + 1, NULL, &fdset, NULL, &select_timeout) > 0) {
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
            if (error == 0) {
                printf("[+] Port %d is OPEN.\n", port);
                open_ports++;
            }
        }

        close(sock);
    }

    printf("\n[#] Scanning Complete. Found %d open ports.\n", open_ports);
    printf("[!] Press <ENTER> to Quit ");
    getchar();
    getchar();
    return 0;
}