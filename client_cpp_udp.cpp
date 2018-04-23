//
// Created by jday on 4/20/18.
//

#include <stdio.h>
#include <sys/time.h>
// Server side C/C++ program to demonstrate Socket programming
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>
#define PORT 8080


const int segmentSize = 512;
const int HALF_SECOND_MICROSECONDS=500000;
const int FULL_SECOND_MICROSECONDS=1000000;





void sendACK(int socketDescriptor, struct sockaddr_in address){
    char message [3];
    message[0]='A';
    message[1]='C';
    message[2]='K';


    if(sendto(socketDescriptor,message, sizeof(message), MSG_CONFIRM, (const struct sockaddr *) &address,sizeof(address)) < 0){
        printf("Failed to send ACK. Terminating. \n");
        exit(1);

    }


}


bool receiveACK(int socketDescriptor, int timeOut){

    timeval *opt = new timeval;
    opt->tv_sec=2;
    opt->tv_usec=10000;

    if(setsockopt(socketDescriptor, SOL_SOCKET,SO_RCVTIMEO,opt, sizeof(*opt) ) < 0){

        printf ("socket opt failure\n");
        exit(1);
    }


    char* ack = new char [3];
    if(recv(socketDescriptor,&ack,20, 0) < 0){
        printf ("receive failure\n");
        return false;
    }else{
        return true;
    }
}

void sendLength(int socketDescriptor,int length, struct sockaddr_in address){
    int* ptr =&length;
    sendto(socketDescriptor,ptr, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &address,sizeof(address));

}




int getLengthMessage(int socketDescriptor){



    int* length;
    if(recv(socketDescriptor,length,sizeof(length), 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }
    return *length;

}




int sendLengthMessage(int socketDescriptor, int *length){
    if(recv(socketDescriptor,length,sizeof(length), 0) < 0){
        printf ("receive failure\n");
        exit(1);
    }
    return *length;
}

std::string receiveSegment(int socketDescriptor){



}



std::string receiveMessage(int socketDescriptor,struct sockaddr_in address){



    int messageLength = getLengthMessage(socketDescriptor);
    sendACK(socketDescriptor, address);
    std::string entireMessage;
    std::string currentMessage;
    std::string previousMessage;
    while(entireMessage.length() < messageLength) {
        currentMessage = receiveSegment(socketDescriptor);
        sendACK(socketDescriptor, address);
        if(currentMessage != previousMessage){
            entireMessage.append(currentMessage);
            previousMessage = currentMessage;
        }
    }


}
void sendCommand(int socketDescriptor, std::string message,struct sockaddr_in address){

    int length = message.length();

    sendLength(socketDescriptor,length,address);

    if(sendto(socketDescriptor, (message.c_str()), message.length(), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)) < 0){
        //printf("Failed to send command. Terminating. \n");
        //exit(1);

    }
    int messagesSent = 0;
    bool ackStatus = receiveACK(socketDescriptor,FULL_SECOND_MICROSECONDS);

    while(!ackStatus && messagesSent <3){
        sendLength(socketDescriptor,length, address);
        if(sendto(socketDescriptor, (char*)(message.c_str()), message.length(), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)) < 0){

        }
            std::cout<< "message sent "<< messagesSent;
        ackStatus = receiveACK(socketDescriptor,FULL_SECOND_MICROSECONDS);
        ++messagesSent;
    }


/*
    int lowerBound;
    int upperBound;
    while(messagesRemaining>0){
        lowerBound = messagesSent * segmentSize;
        upperBound = (1+ messagesSent )* segmentSize;
        sendSegment(socketDescriptor,message.substr(lowerBound,upperBound));


    }
*/

}








/*
int sendMessage(int socketDescriptor){
    timeval opt;
    opt.tv_sec=0;
    opt.tv_usec=HALF_SECOND_MICROSECONDS;

    if(setsockopt(socketDescriptor, SOL_SOCKET,SO_RCVTIMEO,&opt, sizeof(opt) ) < 0){

        printf ("socket opt failure\n");
        exit(1);
    }

}

*/






void askUser(int *port, char* address, char* command ){
     // printf("Enter server name or IP address: ");
     // scanf("%s", address);

     // printf("Enter port:");
    size_t comSize = 200 * sizeof(char);

//std::cin>>*port;

    printf("Enter command:");
//    std::cin.ignore();
    getline(&command,&comSize,stdin);

    if(*port > 65535){
        printf ("Invalid port number.\n");
        exit(1);
    }

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
    //askUser(port,addressInput,command);

    int socketIdentifier;
    struct sockaddr_in address;

    memset(&address, '0', sizeof(address));
    socketIdentifier = socket(AF_INET, SOCK_DGRAM, 0);

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

/*
    std::string fileName;
command = parseCommand(command,fileName);
*/std::string abc("hello");
    command = (char*)abc.c_str();
sendCommand(socketIdentifier,std::string(command), address);

//    sendto(socketIdentifier, ptr, sizeof(ptr), MSG_CONFIRM, (const struct sockaddr *) &address, (socklen_t*)sizeof(address));
/*
    if(send(socketIdentifier ,ptr,sizeof(ptr) , 0 ) < 0){
        printf("Failed to send command. Terminating. \n");
        exit(1);

    }*/


    return 0;
}