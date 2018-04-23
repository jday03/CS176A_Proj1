//
// Created by jday on 4/20/18.
//



#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <stdlib.h>
#include <iostream>







void sendFile(FILE* file, int socketDescriptor){
    // one char for telling if done or not

    std::string output;
    while(!feof(file)){
        output+=(char)fgetc(file);
    }

size_t size=output.length();


    const size_t sendSize = 1000;
    const size_t firstBufferSize = sendSize+1;

    // ASSUMING TEMPORARILY THAT SINGLE DIGIT MESSAGES
    int messages = size/sendSize;
    if(size % sendSize !=0){
        ++messages;
    }


    char* indicator = new char;
    *indicator= (char) messages;
    output.insert(0,indicator);



    std::cout << "sending file of size: "<< size << " with messages: " << messages<< std::endl;


    int messagesTotal = messages;

    int lowerBound = 0;
    int upperBound = firstBufferSize;
    std::cout<< "lower bound is: " << lowerBound<< " and upper bound is: " << upperBound << std::endl;
    std::string toBeSent1 = output.substr(0,firstBufferSize);
    std::cout<< "message is: " << toBeSent1<< std::endl;

    if(messages > 1) {
    if (send(socketDescriptor, output.substr(0, firstBufferSize).c_str(), (size_t) (firstBufferSize), 0) < 0) {
        printf("File transmission failed. \n");
        exit(1);

    }
        messages--;

    while (messages > 1) {
        lowerBound = firstBufferSize + (messagesTotal - messages - 1) * sendSize;
        upperBound = firstBufferSize + (messagesTotal - messages) * sendSize;

        std::cout << "lower bound is: " << lowerBound << " and upper bound is: " << upperBound << std::endl;

        std::string toBeSent = output.substr(lowerBound,
                                             upperBound);
        std::cout << "message is: " << toBeSent << std::endl;

        if (send(socketDescriptor, output.substr(lowerBound,upperBound).c_str(),
                 (size_t) (sendSize * sizeof(char)), 0) < 0) {
            printf("File transmission failed. \n");
            exit(1);
        }
        messages--;

    }
    if (messages == 1) {
        lowerBound = firstBufferSize + (messagesTotal - messages - 1) * sendSize;
        upperBound = output.length()+5;


        std::string toBeSent = output.substr(lowerBound,
                                             upperBound);
        std::cout << "message is: " << toBeSent << std::endl;
        std::cout << "message length is: " << toBeSent.length() << std::endl;

        std::cout << "lower bound is: " << lowerBound << " and upper bound is: " << upperBound << std::endl;


        std::cout <<std::endl << std::endl<< output<< std::endl;
        if (send(socketDescriptor, output.substr(lowerBound,
                                                 upperBound).c_str(), (size_t) (sendSize * sizeof(char)), 0) <
            0) {
            printf("File transmission failed. \n");
            exit(1);
        }

    }
}
//if there is only one message to send
     else{

    lowerBound = 0;
    upperBound = output.length()-1;
        std::string toBeSent = output.substr(lowerBound,upperBound);
    std::cout << "message is: " << toBeSent << std::endl;
    std::cout << "message length is: " << toBeSent.length() << std::endl;

    std::cout << "lower bound is: " << lowerBound << " and upper bound is: " << upperBound << std::endl;

    std::cout << "lower bound is: " << lowerBound << " and upper bound is: " << upperBound << std::endl;
    if (send(socketDescriptor, output.substr(lowerBound, upperBound).c_str(), (size_t) (sendSize * sizeof(char)), 0) <
        0) {
        printf("\nsend Failed \n");
        exit(1);
    }

}

}


int main(int argc, char *argv[]) {




    int socketIdentifier;
    std::string portText(argv[1]);
    const int port = std::stoi(portText);
    if(port > 65535){
        printf ("Invalid port number.\n");
        exit(1);
    }
    int opt = 1;
    struct sockaddr_in address;

    socketIdentifier = socket(AF_INET, SOCK_DGRAM, 0);

    if(socketIdentifier < 0){
        printf ("socket creation failure\n");

        exit(1);
    }


    if(setsockopt(socketIdentifier, SOL_SOCKET,SO_REUSEPORT,&opt, sizeof(opt) ) < 0){

        printf ("socket opt failure\n");
        exit(1);
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 8080);
    printf("port is: %d ", address.sin_port);

    if( bind(socketIdentifier, (struct sockaddr *)& address,sizeof(address)) < 0){
        printf ("bind failure\n");
        exit(1);
    }




    while(true) {
        printf("LISTENING\n");

        if (listen(socketIdentifier, 1) < 0) {
            printf("listen failure\n");
            exit(1);
        }
        printf("HERE1 \n");
        int addressLength = sizeof(address);

        int new_socket = accept(socketIdentifier, (struct sockaddr *) &address, (socklen_t *) &addressLength);

        if (new_socket < 0) {
            printf("socket acceptance failure\n");
            exit(1);

        }
        printf("HERE2 \n");

        char command[3000];
        if (recv(new_socket, &command, sizeof(command), 0) < 0) {
            printf("Failed getting instructions from the client. \n");
            exit(1);
        }



        char test[30] = "cat test.txt";
        std::string comm (command);
        comm+= " 2>&1";
        std::cout << comm <<std::endl;
        FILE *ptr = popen(comm.c_str(), "r");




        sendFile(ptr, new_socket);
    }

}