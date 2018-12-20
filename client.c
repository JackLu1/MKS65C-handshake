#include "pipe_networking.h"

int to_server;
int from_server;

void sighandler(int sig){
    remove("private");
    close(to_server);
    printf("Client has disconnected\n");
    exit(0);
}

int main() {

    signal(SIGINT, sighandler);
    char * buffer = calloc(1,100);
    from_server = client_handshake( &to_server );

    while(1){
        printf("\n\n");
        printf("enter message: ");
        scanf("%s", buffer);
        write(to_server, buffer, 100);
        read(from_server, buffer, 100);
        printf("SERVER RESPOND: %s\n", buffer);
    }
}
