//
// Created by jday on 4/20/18.
//

#include <stdio.h>
// Server side C/C++ program to demonstrate Socket programming
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>
#define PORT 8080


class string;

void askUser(int *port, char* address, char* command ){
    //  printf("Enter server name or IP address: ");
    //  scanf("%s", address);

    //  printf("Enter port:");
    //  scanf("%d", port);
    printf("Enter command:");
    //scanf("%s", command);
    size_t comSize = 200 * sizeof(char);
    getline(&command,&comSize,stdin);

}

void receiveFile(int socketDescriptor, std::string fileName){
    std::cout << "receiving file";
    FILE * newFile= fopen(fileName.c_str(),"w");

    int messagesRemaining;
    const int bufferSize= 60;
    const int firstBufferSize = bufferSize+1;
    char* buffer=new char[firstBufferSize];


    if(recv(socketDescriptor,buffer,bufferSize*sizeof(char) , 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }



    messagesRemaining = (int)buffer[0];
    char* fullBuffer = new char[messagesRemaining*bufferSize];

    for(int i = 0;i < bufferSize;++i ){
        fullBuffer[i]=buffer[i+1];
    }

    delete[] buffer;

    buffer = new char[bufferSize];

    int messagesTotal = messagesRemaining;
    --messagesRemaining;
    while(messagesRemaining > 0){

        if(recv(socketDescriptor,buffer,bufferSize*sizeof(char) , 0) < 0){
            printf ("receive failure\n");
            exit(1);
        }

        for(int i = 0;i < bufferSize;++i ){
            int item = bufferSize*(messagesTotal-messagesRemaining)+i;
            fullBuffer[bufferSize*(messagesTotal-messagesRemaining)+i]=buffer[i];
        }

        --messagesRemaining;
    }
    std::cout<< "message is: " << fullBuffer << std::endl;

    fprintf(newFile,"%s",fullBuffer);
fclose(newFile);
}
//assuming that buffer is empty


char* parseCommand(char*command, std::string& file){
    size_t index = 0;
    std::string parse(command);
    std::string result;
    index = parse.find_last_of(">");

    char* output = new char[index];

    for(int count = 0; count < index; ++count){
        output[count] = command[count];
    }

    file= parse.substr(index+1,parse.length());
    return output;
}


int main() {

    int* port = (int*)malloc(sizeof(int));
    char* addressInput = (char*)malloc(100* sizeof(char));
    char* command = (char*)malloc(200* sizeof(char));
    askUser(port,addressInput,command);
    *port = 8080;

    int socketIdentifier;
    struct sockaddr_in address;

    memset(&address, '0', sizeof(address));
    socketIdentifier = socket(AF_INET, SOCK_STREAM, 0);

    if(socketIdentifier < 0){
        printf ("socket creation failure\n");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)<=0)
    {
        printf("Invalid address \n");
        exit(1);
    }

    std::string fileName;
command = parseCommand(command,fileName);


std::cout<< command << std::endl;
std::cout<< fileName << std::endl;
    printf("port is: %d ", address.sin_port);


    if (connect(socketIdentifier, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(1);
    }


    if(send(socketIdentifier ,(char*) command,(size_t) 200* sizeof(*command) , 0 ) < 0){
        printf("\nsend Failed \n");
        exit(1);

    }

    receiveFile(socketIdentifier,fileName);
/*    if(recv(socketIdentifier,  &file, 200*sizeof(file), 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }

    char fileCopy[300];
    fgets((char*)fileCopy,100,(FILE*)&file );
    printf("received: %s \n",fileCopy);
*/

    return 0;
}