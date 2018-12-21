#include "pipe_networking.h"
#define WKP "public"


/*=========================
  server_handshake
args: int * to_client

Performs the server side pipe 3 way handshake.
Sets *to_client to the file descriptor to the downstream pipe.

returns the file descriptor for the upstream pipe.
=========================*/
int server_handshake(int *to_client) {
    remove("public");
    if ( mkfifo(WKP, 0655) < 0 ){
        perror("wkp");
        return -1;
    }

    printf("open WKP waiting for client\n");
    int upstream = open(WKP, O_RDONLY);
    if (upstream < 0){
        perror("upstream");
        return -1;

    }


    char buffer[256];
    if (read(upstream, buffer, 256) < 0){
        perror("read");
        return -1;
    }
    printf("got private pipe name %s from client\n", buffer);

    //to_client is downstream - server send to cli
    *to_client = open(buffer, O_WRONLY);
    if (*to_client < 0){
        perror("client");
        return -1;
    }

    if ( write(*to_client, "acknowledge client", 20) < 0 ){
        perror("server write");
        return -1;
    }

    if (read(upstream, buffer, 256) < 0){
        perror("read third");
        return -1;
    }

    printf("third ack from client %s\n", buffer);

    return upstream;
}

/*=========================
  client_handshake
args: int * to_server

Performs the client side pipe 3 way handshake.
Sets *to_server to the file descriptor for the upstream pipe.
returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {
    char * private = "private";
    remove("private");
    if(mkfifo(private, 0655) < 0){
        perror("private");
        return -1;
    }
    printf("client made private pipe\n");

    // send upstream to server
    *to_server = open(WKP, O_WRONLY);
    if (*to_server < 0){
        perror("server");
        return -1;
    }
    printf("client connected to WKP\n");
    
    //send private pipe to server
    if (write(*to_server, "private", 8) < 0){
        perror("write");
        return -1;
    }
    printf("client wrote to WKP\n");

    int downstream = open(private, O_RDONLY);
    if ( downstream < 0){
        perror("private write");
        return -1;
    }

    char buffer[256];
    if (read(downstream, buffer, 256) < 0){
        perror("client read");
        return -1;
    }
    printf("server sent: %s\n", buffer);

    if (write(*to_server, "third", 8) < 0){
        perror("write");
        return -1;
    }

    return downstream;
}
