#include "pipe_networking.h"

int to_client;
int from_client;

void sighandler(int sig){
    if (sig == SIGINT){
        remove("public");
        printf("Server has disconnected\n");
        exit(0);
    } else if (sig == SIGPIPE){
        remove("public");
        printf("client has disconnected\n");
        exit(0);
    }
}


int main() {

    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    char * buffer = calloc(1,100);
    from_client = server_handshake( &to_client );

    while(1){
        printf("\n\n");
        printf("Awaiting client...\n");
        memset(buffer, 0, 100);
        read(from_client, buffer, 100);
        printf("client sent:%s\n", buffer);
        sprintf(buffer, "%s!", buffer);
        write(to_client, buffer, 100);

    }
}
