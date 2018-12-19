#include "pipe_networking.h"

#define WKP "namedpipe"


/*=========================
  server_handshake
args: int * to_client

Performs the client side pipe 3 way handshake.
Sets *to_client to the file descriptor to the downstream pipe.

returns the file descriptor for the upstream pipe.
=========================*/
int server_handshake(int *to_client) {
    // setup
    remove("namedpipe");
    if ( mkfifo(WKP, 0655) < 0){
        perror("wkp");
        return -1;
    }
    printf("made WKP\n");

    printf("open WKP waiting for client\n");
    int downstream = open(WKP, O_RDONLY);
    if (downstream < 0){
        perror("downstream");
        return -1;
    }

    char buffer[256];
    if (read(downstream, buffer, 256) < 0){
        perror("read");
        return -1;
    }
    printf("got private pipe name %s from client\n", buffer);



    //connect to private client 
    int client = open(buffer, O_WRONLY);
    if (client < 0){
        perror("client");
        return -1;
    }

    if ( write(client, "acknowledge client", 20) < 0 ){
        perror("server write");
        return -1;
    }

    if (read(downstream, buffer, 256) < 0){
        perror("read third");
        return -1;
    }

    printf("third ack from client %s\n", buffer);

    close(downstream);
    remove("namedpipe");
    return 0;
}


/*=========================
  client_handshake
args: int * to_server

Performs the client side pipe 3 way handshake.
Sets *to_server to the file descriptor for the upstream pipe.
returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {
    // setup
    char * private = "private";
    remove("private");
    if ( mkfifo(private, 0655) < 0 ){
        perror("private");
        return -1;
    }
    printf("client made private pipe\n");

    //2a
    int server = open(WKP, O_WRONLY);
    if (server < 0){
        perror("server");
        return -1;
    }
    printf("client connected to WKP\n");

    if (write(server, "private", 8) < 0){
        perror("write");
        return -1;
    }
    printf("client wrote to WKP\n");

    printf("client wait for server ack\n");
    int p_pipe = open(private, O_RDONLY);
    if ( p_pipe < 0){
        perror("private write");
        return -1;
    }

    char buffer[256];
    if (read(p_pipe, buffer, 256) < 0){
        perror("client read");
        return -1;
    }
    printf("server sent: %s\n", buffer);

    close(p_pipe);
    remove("private");

    if (write(server, "third", 8) < 0){
        perror("write");
        return -1;
    }

    return 0;
}
