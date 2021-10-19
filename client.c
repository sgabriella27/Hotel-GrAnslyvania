#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>

int main(int argc, char** args){
    int sock_fd;
    struct sockaddr_in client;

    if(argc < 3){
        perror("Argument or Parameter Must be 3");
        return -1;
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        perror("Create Socket Failed");
        return -1;
    }

    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(atoi(args[2]));

    if(isalpha(args[1][0]) > 0){
        inet_pton(AF_INET, args[1], client.sin_addr.s_addr);
    }
    else if(isdigit(args[1][0]) > 0){
        client.sin_addr.s_addr = inet_addr(args[1]);
    }
    else{
        perror("IP Address Input Type Invalid");
        return -1;
    }

    if(connect(sock_fd, (struct sockaddr*)&client, sizeof(client)) < 0){
        perror("Connecting to Server Error");
        return -1;
    }

    char fullName[100] = {0}, phoneNumber[100] = {0}, roomType[20] = {0};
    int age = 0, nightStay = 0, len = 0, check = 0;

    char data[255] = {0}, bookingData[255] = {0}, bookingID[10] = {0}, msg[100] = {0};
    int totalPrice = 0, breakfast = 0;

    while (1)
    {
        printf("+===============================+\n");
        printf("| Welcome to Hotel GrAnsylvania |\n");
        printf("+===============================+\n\n");

        //Client Validation

        //Full Name Validation
        do{
            printf("Input Full Name [3..25] [""Exit"" to Close Program (Case Sensitive)] : ");
            scanf("%[^\n]", fullName); while (getchar() != '\n');
            len = strlen(fullName);
            if(len < 3 || len > 25){
                printf("Full name length must between 3 and 25\n");
                check = 1;
            }
            else{
                check = 0;
            }
        }
        while(check == 1);

        //Exit Validation
        if(strcmp(fullName, "Exit") == 0){
            send(sock_fd, fullName, strlen(fullName), 0);
            if(read(sock_fd, &msg, sizeof(msg)) < 0){
                perror("Read data from server error");
                return -1;
            }
            printf("%s\n", msg);
            send(sock_fd, "oke", 3, 0);
            break;
        }


        //Phone Number Validation
        do{
            printf("Input Phone Number [Must Begin With '+62 '] : ");
            scanf("%[^\n]", phoneNumber); while (getchar() != '\n');
            len = strlen(phoneNumber);
            if(phoneNumber[0] != '+' || phoneNumber[1] != '6' || phoneNumber[2] != '2' || phoneNumber[3] != ' '){
                printf("Phone number must begin with '+62 '\n");
                check = 1;
            }
            else{
                check = 0;
            }
        }
        while(check == 1);

        //Age Validation
        do{
            printf("Input Your Age [Minimum 18] : ");
            scanf("%d", &age); while (getchar() != '\n');
            if(age < 18){
                printf("Age must be minimum 18\n");
                check = 1;
            }
            else{
                check = 0;
            }
        }
        while (check == 1);

        //Room Type Validation
        do{
            printf("Input Room Type [Regular | Deluxe | Suite] (Case Sensitive) : ");
            scanf("%[^\n]", roomType); while (getchar() != '\n');
            if(strcmp(roomType, "Regular") != 0
                && strcmp(roomType, "Deluxe") != 0
                && strcmp(roomType, "Suite") != 0){
                printf("Room type must be either Regular or Deluxe or Suite (Case Sensitive)\n");
                check = 1;
            }
            else{
                check = 0;
            }
        }
        while (check == 1);

        //Night Stay Validation
        do{
            printf("Input How Many Night You Will Stay [1..30] : ");
            scanf("%d", &nightStay); while (getchar() != '\n');
            if(nightStay < 1 || nightStay > 30){
                printf("You can't stay less than 1 night or more than 30 nights\n");
                check = 1;
            }
            else{
                check = 0;
            }
        }
        while (check == 1);

        sprintf(data, "%s#%s#%d#%s#%d", fullName, phoneNumber, age, roomType, nightStay);

        send(sock_fd, data, strlen(data), 0);
        printf("Data has been send to server...\n");
        printf("Press ENTER to view booking detail...\n");
        while (getchar() != '\n');

        if(read(sock_fd, &bookingData, sizeof(bookingData)) < 0){
            perror("Read Data From Server Failed");
            return -1;
        }
        
        char *resultData = strtok(bookingData, "#");
        strcpy(bookingID, resultData);
        resultData = strtok(NULL, "#");
        breakfast = atoi(resultData);
        resultData = strtok(NULL, "#");
        totalPrice = atoi(resultData);

        printf("+================================+\n");
        printf("| Booking ID   : %-15s |\n", bookingID);
        printf("+================================+\n");
        printf("| Full Name    : %-15s |\n", fullName);
        printf("| Phone Number : %-15s |\n", phoneNumber);
        printf("| Room Type    : %-15s |\n", roomType);
        printf("| Night Stay   : %02d %-12s |\n", nightStay, "night(s)");
        if(breakfast == 1){
            printf("| Breakfast    : %-15s |\n", "Not Included");
        }
        else{
            printf("| Breakfast    : %-15s |\n", "Included");
        }
        printf("+================================+\n");
        printf("| Total Price  : Rp. %-11d |\n", totalPrice);
        printf("+================================+\n\n");
        printf("Press ENTER to continue...\n");
        while (getchar() != '\n');
    }

    close(sock_fd);

    return 0;
}