#include "pipe_networking.h"

int to_client;
int from_client;
int main();

void sighandler(int sig){
    if (sig == SIGINT){
        remove("public");
        printf("Server has disconnected\n");
        exit(0);
    } else if (sig == SIGPIPE){
        close(from_client);
        close(to_client);
        printf("from %i\n", from_client);
        printf("client has disconnected\n");
        main();
    }
}


int main() {

    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    char * buffer = calloc(1,100);
    from_client = server_handshake( &to_client );

    while(1){
        printf("\n\n");
        printf("from %i to %i\n", from_client, to_client);
        printf("Awaiting client...\n");
        memset(buffer, 0, 100);
        read(from_client, buffer, 100);
        printf("client sent:%s\n", buffer);
        sprintf(buffer, "%s!", buffer);
        write(to_client, buffer, 100);

    }
}
