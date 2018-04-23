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

void askUser(int *port, char* address, char* command ){
      printf("Enter server name or IP address: ");
      scanf("%s", address);

      printf("Enter port:");
    size_t comSize = 200 * sizeof(char);

std::cin>>*port;

    printf("Enter command:");
    std::cin.ignore();
    getline(&command,&comSize,stdin);

    if(*port > 65535){
        printf ("Invalid port number.\n");
        exit(1);
    }

}

void receiveFile(int socketDescriptor,std::string fileName){
    int messagesReceived = 1;
    std::cout << "receiving file";
    //
    int messagesRemaining;
    const int bufferSize= 1000;
    const int firstBufferSize = bufferSize+1;
    char* initialBuffer=new char[firstBufferSize];


    if(recv(socketDescriptor,initialBuffer,firstBufferSize , 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }


    std::string fullBuffer(initialBuffer);
    messagesRemaining = (int)initialBuffer[0];
    fullBuffer=fullBuffer.substr(1,fullBuffer.length());


    int messagesTotal = messagesRemaining;
    --messagesRemaining;
    while(messagesRemaining > 0){
        char buffer[bufferSize];
        std::cout<< "message is currently: " << fullBuffer << std::endl;

        if(recv(socketDescriptor,buffer,bufferSize, 0) < 0){
            printf ("receive failure\n");
            exit(1);
        }
        std::string buffStr (buffer);
        int offset = 2;
        if(messagesRemaining == 1){
            offset = 1;
        }
        buffStr=buffStr.substr(0,buffStr.length()-offset);
        fullBuffer.append(buffStr);

               --messagesRemaining;
        messagesReceived++;
    }
    std::cout<< "message is: " << fullBuffer << std::endl;

    FILE * newFile= fopen(fileName.c_str(),"w");
    fprintf(newFile,"%s",fullBuffer.c_str());
    fclose(newFile);
    std::cout<< "File "<< fileName<< " saved.";

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

    char* newParse = strtok((char*)parse.c_str(), "\n");

    file= std::string(newParse).substr(index+1,parse.length());
    return output;
}


int main() {

    int* port = (int*)malloc(sizeof(int));
    char* addressInput = (char*)malloc(100* sizeof(char));
    char* command = (char*)malloc(200* sizeof(char));
    askUser(port,addressInput,command);

    int socketIdentifier;
    struct sockaddr_in address;

    memset(&address, '0', sizeof(address));
    socketIdentifier = socket(AF_INET, SOCK_STREAM, 0);

    if(socketIdentifier < 0){
        printf ("Could not connect to server.\n");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr)<=0)
    {
        printf("Could not connect to server. \n");
        exit(1);
    }

    std::string fileName;
command = parseCommand(command,fileName);


std::cout<< command << std::endl;
std::cout<< fileName << std::endl;
    printf("port is: %d ", address.sin_port);


    if (connect(socketIdentifier, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Could not connect to server. \n");
        exit(1);
    }


    if(send(socketIdentifier ,(char*) command,(size_t) 200* sizeof(*command) , 0 ) < 0){
        printf("Failed to send command. Terminating. \n");
        exit(1);

    }

    receiveFile(socketIdentifier,fileName);


/*
    if(recv(socketIdentifier,  &file, 200*sizeof(file), 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }

    char fileCopy[300];
    fgets((char*)fileCopy,100,(FILE*)&file );
    printf("received: %s \n",fileCopy);
*/

    return 0;
}