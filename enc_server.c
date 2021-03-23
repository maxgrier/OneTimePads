/******************************
* Author: Max Grier
* Course: CS344 - 400 - W2021
* Date Modified: 3-8-2021
* Due Date: 3-8-2021
* Description: This will send the encrypted message to the enc_client.
* Source code credit: Parts of this code are either from, or based on,
*                     the code in the modules. This was heavily influenced by the sample file.
******************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define MAX_BUFF 150000
#define ASCII_OFFSET 65
#define MODULO 27

// Error function used for reporting issues
void error(const char* msg) {
    perror(msg);
    exit(1);
}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char* argv[]) {
    int connectionSocket, charsRead;
    int start_key, txt_num, key_num, enc_num;
    char buffer[MAX_BUFF];

    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Set up for sending the plain txt
    char* plain_send = calloc((MAX_BUFF) + 1, sizeof(char));
    int plain_send_len = 0;

    // Set up for sending the key values
    char* key_send = calloc((MAX_BUFF) + 1, sizeof(char));

    // Set up for encrypted message
    char* enc_mesg = calloc((MAX_BUFF) + 1, sizeof(char));
    int enc_msg_len = 0;

    // Set up for the complete message sent
    char* total_send = calloc((MAX_BUFF) + 1, sizeof(char));
    int tot_send_len = 0;


    // Child variables for the forking
    int childPid = 0;
    int childStatus = 0;

    // Begin the forking for the processes
    pid_t spawnpid = -5;
    spawnpid = fork();
    switch (spawnpid) {
        case -1:
            perror("fork() failed!");
            exit(1);
        case 0:
            // Check usage & args
            if (argc < 2) {
                fprintf(stderr, "USAGE: %s port\n", argv[0]);
                exit(1);
            }

            // Create the socket that will listen for connections
            int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (listenSocket < 0) {
                error("ERROR opening socket");
            }

            // Set up the address struct for the server socket
            setupAddressStruct(&serverAddress, atoi(argv[1]));


            // Associate the socket to the port
            if (bind(listenSocket,
                     (struct sockaddr*)&serverAddress,
                     sizeof(serverAddress)) < 0) {
                error("ERROR on binding");
            }

            // Start listening for connections. Allow up to 5 connections to queue up
            listen(listenSocket, 5);

            // Accept a connection, blocking if one is not available until one connects
            while (1) {
                // Accept the connection request which creates a connection socket
                connectionSocket = accept(listenSocket,
                                          (struct sockaddr*)&clientAddress,
                                          &sizeOfClientInfo);
                if (connectionSocket < 0) {
                    error("ERROR on accept");
                }

                // Get the message from the client and display it
                memset(total_send, '\0', MAX_BUFF);
                // Read the client's message from the socket
                charsRead = recv(connectionSocket, total_send, MAX_BUFF - 1, 0);
                if (charsRead < 0) {
                    error("ERROR reading from socket");
                }

                // Find length of the total message
                tot_send_len = strlen(total_send);

                // Break out the message with plain text and key
                for (int i = 0; i < tot_send_len; i++) {
                    // Look for the separator
                    if (total_send[i] == '@' && total_send[i + 1] == '@') {
                        for (int j = 0; j < i; j++) {
                            // Set the plain text
                            plain_send[j] = total_send[j];
                            plain_send_len++;
                        }
                        // The key starts after plain text
                        start_key = i + 2;
                    }
                }


                // After making the plain text, make the key string
                for (int i = 0; i < (strlen(total_send)) - start_key; i++)
                    key_send[i] = total_send[start_key + i];

                // Create the encryption message for client
                for (int i = 0; i < plain_send_len; i++) {
                    // Covert chars to their int value
                    if(plain_send[i] == ' '){
                        txt_num = plain_send[i] - 6;
                    }else{
                        txt_num = plain_send[i] - ASCII_OFFSET;
                    }
                    if(key_send[i] == ' '){
                        key_num = key_send[i] - 6;
                    }else{
                        key_num = key_send[i] - ASCII_OFFSET;
                    }


                    // Get the encrypted character
                    enc_num = txt_num + key_num;
                    enc_num = enc_num % MODULO;
                    if(enc_num == 26){
                        enc_mesg[i] = 32;
                    }

                    else{
                        // Set the encrypted message
                        enc_mesg[i] = enc_num + ASCII_OFFSET;
                    }


                }



                // Need to: Send a Success message back to the client to test for dec_server
                // Get the encrypted message length
                enc_msg_len = strlen(enc_mesg);
                while (send(connectionSocket, enc_mesg, strlen(enc_mesg), 0) != enc_msg_len) {
                    if (charsRead < 0) {
                        error("ERROR writing to socket");
                    }
                }

                // Close the connection socket for this client
                close(connectionSocket);
            }
            // Close the listening socket
            close(listenSocket);
        default:
            childPid = wait(&childStatus);
            break;
    }
    return 0;
}
