#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<time.h>
#include<ctype.h>

int sock_fd;
struct sockaddr_in server, client;

int randomNumberID(int min, int max){
    return rand()%(max-min) + min;
}

int main(int argc, char** args){
    srand(time(0));
    int optionValue = 1;

    if(argc != 2){
        perror("Argument or Parameter Must Be 2");
        return -1;
    }
    
    printf("+=============================+\n");
    printf("| Hotel GrAnsylvania - Server |\n");
    printf("+=============================+\n\n");
    printf("Server Active!\n\n");

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        perror("Create Socket Failed");
        return -1;
    }

    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)) < 0){
        perror("Set Socket Option Failed");
        return -1;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(args[1]));
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock_fd, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Binding Socket Failed");
        return -1;
    }

    if(listen(sock_fd, 5) < 0){
        perror("Listening to Socket Failed");
        return -1;
    }

    int client_size = sizeof(client);
    int new_socket = accept(sock_fd, (struct sockaddr*)&client, &client_size);

    
    while (1)
    {

        char data[255] = {0};
        char fullName[100] = {0}, phoneNumber[100] = {0}, roomType[20] = {0};
        int nightStay, price = 0, totalPrice = 0, breakfast = 0;
        char bookingID[10] = {0}, bookingData[255] = {0};
        char msg[200] = {"\n+============================+\n| Thankyou and See You Soon! |\n+============================+\n"};
        char recv[100] = {0};

        if(new_socket < 0){
            perror("Accepting Socket Failed");
            return -1;
        }

        if(read(new_socket, &data, sizeof(data)) < 0){
            perror("Read From Client Failed");
            return -1;
        }

        if(strcmp(data, "Exit") == 0){
            send(new_socket, msg, strlen(msg), 0);
            if(read(new_socket, &recv, sizeof(recv)) < 0){
                perror("Error reading from client");
                return -1;
            }
            close(new_socket);
            break;
        }

        char *cusData = strtok(data, "#");
        strcpy(fullName, cusData);
        cusData = strtok(NULL, "#");
        strcpy(phoneNumber, cusData);
        cusData = strtok(NULL, "#");
        cusData = strtok(NULL, "#");
        strcpy(roomType, cusData);
        cusData = strtok(NULL, "#");
        nightStay = atoi(cusData);

        //Server Validation

        //Random BookingID
        int numberID1 = randomNumberID(0, 10);
        int numberID2 = randomNumberID(0, 10);
        sprintf(bookingID, "%c%c-%c%c%d%d", toupper(roomType[0]), toupper(roomType[1]), 
                                            toupper(fullName[0]), toupper(fullName[1]), 
                                            numberID1, numberID2);
        
        //Room Price and Breakfast Validation
        if(strcmp(roomType, "Regular") == 0){
            price = 350000;
            if(nightStay < 7){
                breakfast = 1;
            }
            else{
                breakfast = 0;
            }
        }
        else if(strcmp(roomType, "Deluxe") == 0){
            price = 700000;
            if(nightStay < 4){
                breakfast = 1;
            }
            else{
                breakfast = 0;
            }
        }
        else if(strcmp(roomType, "Suite") == 0){
            price = 1000000;
            breakfast = 0;
        }

        //Calculate Total Price
        totalPrice = price * nightStay;

        sprintf(bookingData, "%s#%d#%d", bookingID, breakfast, totalPrice);
        
        send(new_socket, bookingData, strlen(bookingData), 0);
    }
    
    close(sock_fd);

    return 0;
}