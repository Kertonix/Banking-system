#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//local
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");



    char buff[MAX];
        int n;	//auxiliary variables
        int counter = 0;


        while(1) { // login loop
    			bzero(buff, sizeof(buff));
    			printf("Podaj login: ");
    			n = 0;
    			scanf("%s",buff);

    			write(sockfd, buff, sizeof(buff)); // sends login to server
    			bzero(buff, sizeof(buff));
    			read(sockfd, buff, sizeof(buff)); // server response
    			if('y' == buff[0]){
    				counter = 0;
    				n = 1;
    				break;
    			}
    			else if('x' == buff[0]){
    				printf("Serwer rozłączył się \n");	//disconnected
    				counter = 0;
    				n = 0;
    				break;
    			}
    			else{
    				printf("Błędny login\n");	//bad login
    				counter++;
    			}
    			if(counter==4){		//too many attempts
    				printf("przekroczyłeś limit 4 prób podania loginu \n blokada konta\n");
    				n = 0;
    				write(sockfd,"x", 1);
    				break;
    			}
        	}

        counter = 0;
    	while(n){	//password loop

    		printf("\nProszę o podanie hasła: ");
    		bzero(buff, sizeof(buff));
    		n = 1;
    		scanf("%s",buff);

    		write(sockfd, buff, sizeof(buff)); // send password
    		bzero(buff, sizeof(buff));
    		read(sockfd, buff, sizeof(buff)); // server response
    		if('p'==buff[0]){
    			counter = 1;
    			printf("Zalogowany poprawnie\n");	//login succeful
    			break;
    		}
    		else if('x' == buff[0]){
    			printf("Serwer rozłączył się \n");	//disconnected
    			counter = 0;
    			n = 0;
    			break;
    		}
    		else{
    			printf("Błędne hasło\n");	//incorrect password
    			counter++;
    		}
    		if(counter==4){	//too many attempts
    			printf("przekroczyłeś limit 4 prób podania loginu \n blokada konta\n");
    			write(sockfd,"x", 1);
    			counter = 0;
    			break;
    		}

    	}

    	n = 1;
    	if(counter==1){	//transaction loop
    		while(n){

				bzero(buff, MAX);
				puts("\nCo chcesz zrobić?");
				puts("1 - Sprawdź stan konta");
				puts("2 - wpłać pieniądze do konta");
				puts("3 - wypłać pieniądze z konta");
				puts("4 - wyloguj się");
				puts("Wybieraj: ");
				scanf("%s",buff);

				int amount;

						switch (buff[0]){
							case '1':	//check balance

								write(sockfd, buff, strlen(buff));
								bzero(buff, sizeof(buff));
								puts("\nStan konta to: ");
								read(sockfd, &amount, sizeof(int));
								printf("%d",amount);
								break;
							case '2':	//deposit
								write(sockfd, buff, strlen(buff));
								bzero(buff, sizeof(buff));

								puts("Ile pieniędzy chcesz wpłacić ?");
								scanf("%d",&amount);
								write(sockfd,&amount, sizeof(int));
								read(sockfd, buff, sizeof(buff));
								if(buff[0]=='y'){
									printf("Wpłacono %d", amount);
								}else{
									printf("Błędna wartość %d - podaj liczbę dodatnią", amount);
								}
								bzero(buff, sizeof(buff));

								break;
							case '3':	//withdraw
								write(sockfd, buff, strlen(buff));
								puts("Ile pieniędzy chcesz wypłacić ?");
								scanf("%d",&amount);
								write(sockfd,&amount, sizeof(int));
								bzero(buff, sizeof(buff));
								read(sockfd, buff, sizeof(buff));
								if(buff[0]=='y'){
									puts("Wypłacono");

								}else
								{
									puts("Niepowodzenie, zbyt mało środków na koncie");
								}

								break;
							case '4':	//end of program
								bzero(buff, sizeof(buff));
								puts("\nKoniec programu");
								write(sockfd, "4", 1);
								n = 0;
								break;
							default:	//incorrect option
								puts("0");
								printf("%c",buff[0]);
								puts("1");
								puts("\nProszę o wybranie opcji: '1','2','3' lub '4'\n");
								break;
						}
			}
    	}

    // close the socket
    close(sockfd);
    exit(0);
}
