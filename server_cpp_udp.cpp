//
// Created by jday on 4/20/18.
//



#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>


const int segmentSize = 512;
const int HALF_SECOND_MICROSECONDS=500000;
const int FULL_SECOND_MICROSECONDS=1000000;




void sendACK(int socketDescriptor, struct sockaddr_in address,struct sockaddr_in clientAddress,socklen_t client_Len){
    char* message = new char[3];
    message[0]='A';
    message[1]='C';
    message[2]='K';


    if(sendto(socketDescriptor,message, 30, MSG_CONFIRM, (const struct sockaddr *) &clientAddress,client_Len) < 0){
        printf("Failed to send command. Terminating. \n");
        exit(1);

    }
std::cout<< "ACK SENT";

}


int getLengthMessage(int socketDescriptor, struct sockaddr_in address,struct sockaddr_in& clientAddress,socklen_t& client_Len) {
    memset(&clientAddress, 0, sizeof(clientAddress));

    clientAddress.sin_family    = AF_INET; // IPv4
    clientAddress.sin_port = htons(8080);
    int length;
    if (recvfrom(socketDescriptor, &length, sizeof(length),
                 0, (struct sockaddr *) &clientAddress, &client_Len) > 0) {

        std::cout << "length is " << length << std::endl;
        return length;

    }
}


std::string receiveCommandText(int socketDescriptor, bool timeout, int timeOutTime, int length){
    char* charMessage = new char [400];
    timeval opt;
    opt.tv_sec=10;
    opt.tv_usec=timeOutTime;

    if(setsockopt(socketDescriptor, SOL_SOCKET,SO_RCVTIMEO,&opt, sizeof(opt) ) < 0){

        printf ("socket opt failure\n");
        exit(1);
    }

    if(recv(socketDescriptor,charMessage,400, 0) < 0){
        printf ("Failed getting instructions from the client.\n");
        return "0";
    }
    std::cout<< charMessage << std::endl;

    std::string message(charMessage);
    if (message.length() == length) {
        return message;

    } else {
        printf("Failed getting instructions from the client.\n");
        return "0";
    }


}

std::string receiveCommand(int socketDescriptor,struct sockaddr_in address, struct sockaddr_in clientAddress,socklen_t client_Len) {
    int messageLength = getLengthMessage(socketDescriptor, address,clientAddress,client_Len);
    if (messageLength > 0) {
        std::string result = receiveCommandText(socketDescriptor, true, HALF_SECOND_MICROSECONDS, messageLength);
        if (result == "0") {
            return "0";
        } else {
            sendACK(socketDescriptor,address, clientAddress,client_Len);
            return result;
        }
    } else{
        return "0";
    }
}


int main(int argc, char *argv[]) {


    struct sockaddr_in clientAddress;
    socklen_t client_Len;

    int socketIdentifier;
    std::string portText(argv[1]);
    const int port = std::stoi(portText);
    if(port > 65535){
        printf ("Invalid port number.\n");
        exit(1);
    }
    int opt = 1;
    struct sockaddr_in address;


std::cout<< port;
    socketIdentifier = socket(AF_INET, SOCK_DGRAM, 0);

    if(socketIdentifier < 0){
        printf ("socket creation failure\n");

        exit(1);
    }


    if (setsockopt(socketIdentifier, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {

        printf("socket opt failure\n");
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



        timeval opt;
        opt.tv_sec=0;
        opt.tv_usec=0;

       std::string command = receiveCommand(socketIdentifier,address, clientAddress, client_Len);
        if(command!= "0"){
            //continue the code

            std::string comm (command);
            comm+= " 2>&1";
            std::cout << comm <<std::endl;
            FILE *ptr = popen(comm.c_str(), "r");

            std::string output;
            while(!feof(ptr)){
                output+=(char)fgetc(ptr);
            }
            //sendFile(output);

        }




    }

}