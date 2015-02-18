#include <stdlib.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>

int create_server(int port) {
    int server_socket;

    /* Creation of the Socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(-1);
    }

    /* Enable SO_REUSEADDR */
    int optval = 1;

    if (setsockopt(server_socket,SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
      perror("Can not set SO_REUSEADR option");
      exit(-1);
    }
    
    /* Bind Socket */
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        perror("bind server socket");
        exit(-1);
    }

    /* Enable listening */
    if (listen(server_socket, 10) == -1) {
        perror("listen server socket");
        exit(-1);
    }

    return server_socket;
} 

void signal_treatment(int sig) {
    if (sig == SIGCHLD) {
        int status;
        if (waitpid(-1, &status, WNOHANG) == -1) {
            perror("waitpid");
        }
        if (WIFSIGNALED(status))
            printf("killed by %d\n", WTERMSIG(status));
    }
}

/* Set which signals the program has to compute differently */
void signal_init(void) {
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
    }

    struct sigaction sa;
    sa.sa_handler = signal_treatment;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror ("sigaction(SIGCHLD)");
    }
}
