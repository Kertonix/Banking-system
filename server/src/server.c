#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

//ścieżki do wymaganych plików | paths of mandatory files
char LOGCHAR[] = "/home/piotr/Documents/PULX/PULX eclipse workspace/log.txt";	//log
char USER1CHAR1[] = "/home/piotr/Documents/PULX/PULX eclipse workspace/user1";	//user1
char USER1CHAR2[] = "/home/piotr/Documents/PULX/PULX eclipse workspace/user2";	//user2

// struktura przechowująca w stringach ścieżki do plików z informacjami o userach i logami
// stucture for files path of files containing info about users and logs
struct user {
	char *username;
	char *password;
	int balance;
};


int main(){
	pid_t child;	//zmienna w której będzie przechowywane id procesu potomnego | child pid
//	zmienne gniazda | socket variables
	int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP & PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) < 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification


    struct user client[2];
//	defining client's credentials | definiowanie danych logowania klientów
   	    	client[0].username = "user1";
   	    	client[0].password = "pass1";

   	    	client[1].username = "user2";
   	    	client[1].password = "pass2";


   	    	FILE *file; // log file pointer | pointer na plik logu

   	        char buff[MAX];
   	        int n = 1;
   	        char user[10];

   	        bool CHATTING = true; // server loop

   	while(CHATTING){

		 connfd = accept(sockfd, (SA*)&cli, &len);
			if (connfd < 0) {
				printf("server acccept failed...\n");
				exit(0);
			} else
				printf("server acccept the client...\n");

			if ((child = fork()) == 0) {	//process mgmt
			//child process
			close(sockfd);


		short counter=0;
   	        while(n && counter < 4) { // login input
   	        	bzero(buff, MAX);
   	        	read(connfd, buff, sizeof(buff));
   	        	printf("Podany login: %s ", buff);	//given login

   	            if( (strncmp(client[0].username,buff,5) == 0) || (strncmp(client[1].username,buff,5) == 0) ){	//strings comparison
   	            	counter=0;		//setting counter to 0 - you can turn it off
   	            	write(connfd,"y", 1);
   	            	n = 1;
   	            	strcpy(user,buff);
   	            	printf("Podano poprawny login\n");	//correct login
   	            	break;
   	            }

   	            else if('x' == buff[0]){
   	            	n = 0;
   	            	printf("\nKlient się rozłączył");	//client disconnected
   	            	write(connfd,"x", 1);
   	            }
   	            else{
   	            	write(connfd,"n", 1);
   	            	printf("Podano niepoprawny login\n");	//bad login
   	            	counter++;	//possibility of login input limit
   	            }
   	        }

//   	       checking password & login could be better using below SELECTUSER code in login section

   	        while(n && counter < 4) { // password input
   	                bzero(buff, MAX);	//zeroing the buffer
   	                read(connfd, buff, sizeof(buff));	//reading buffer
       				printf("Podane hasło: %s ", buff);	//given password


   	                if( ((strncmp(client[0].password,buff,6) == 0)&&(strncmp(client[0].username,user,5) == 0)) || ((strncmp(client[1].password,buff,6) == 0)&(strncmp(client[1].username,user,5) == 0)) ){
   	                	write(connfd,"p", 1);
   	                	n = 1;
   	                	printf("Podano poprawne hasło\n");	//correct password
   	                	break;
   	                }
   	                else if(buff[0]=='x'){ //unnecessary its handled by the counter on server side now
   	    				n = 0;
   	    				printf("\nLogowanie nieudane zbyt dużo błędnych prób\n");	//login faild - too many attempts
   	    				write(connfd,"x", 1);
   	    			}
   	                else{
   	                	write(connfd,"n", 1);
   	                	printf("Podano niepoprawne hasło\n");	//incorrect password
   	                	counter++;
   	                }
   	            }
               if(counter==4){
   			   	   printf("\nLogowanie nieudane zbyt dużo błędnych prób\n");	//login faild - too many attempts
               }



  	        if(n!=0){	//if credentials match
   	        	time_t myclock = time(NULL);	//handle the clock
				struct tm *dataclock;
				char mytime[1000];

   	        	file = fopen(LOGCHAR,"a");	//log to file - user logged in
   	        	dataclock = localtime(&myclock);
   	        	strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
   	        	fprintf(file,"\n%s Został zalogowany urzytkownik %s\n",mytime,user);
   	        	printf("\n%s Został zalogowany urzytkownik %s\n",mytime,user);
   	        	fclose(file);



   	        	//selecting the user
   	        	int SELECTEDUSER;
				if(strncmp(user,client[0].username,5) == 0){
					file = fopen(USER1CHAR1,"r");	//reading file for balance check
					fscanf(file,"%d",&client[0].balance);
					fclose(file);
					SELECTEDUSER = 0;
				}else{
					file = fopen(USER1CHAR2,"r");
					fscanf(file,"%d",&client[1].balance);
					fclose(file);
					SELECTEDUSER = 1;
				}

   	    		int amount;
   	    		bool transaction = true; //loop for transaction
   	    		counter=0;
   	    		while(transaction){
//					printf("\ninside transaction\n");	//for debug puprposes only

   	    			bzero(buff, MAX);
					read(connfd, buff, sizeof(buff));
					printf("\nWybrana opcja:  %s\n", buff);

					switch (buff[0]){
						case '1':	//check balance
							bzero(buff, MAX);
							//buff[0] = (char)&client[SELECTEDUSER].balance;
							write(connfd,&client[SELECTEDUSER].balance, sizeof(int));
							printf("Stan konta: %d",client[SELECTEDUSER].balance);
							break;
						case '2':	//deposit
							bzero(buff, MAX);
							read(connfd, &amount, sizeof(int));
							if(amount<0){
								printf("Musisz podać liczbę dodatnią");
								write(connfd,"n", 1);
								break;
							}
							write(connfd,"y", 1);
							client[SELECTEDUSER].balance = client[SELECTEDUSER].balance + amount;
								if(SELECTEDUSER == 0){
									file = fopen(USER1CHAR1,"w");
									fprintf(file,"%d", client[0].balance);
									fclose(file);
								}else{
									file = fopen(USER1CHAR2,"w");
									fprintf(file,"%d",client[1].balance);
									fclose(file);
								}
							fclose(file);
							printf("Zmieniono stan konta na: %d",client[SELECTEDUSER].balance);

							time_t myclock = time(NULL);	//logging
							struct tm *dataclock;
							char mytime[1000];
							file = fopen(LOGCHAR,"a");
							dataclock = localtime(&myclock);
							strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
							fprintf(file,"%s %s wpłacił: %d i posiada: %d\n", mytime,user,amount,client[SELECTEDUSER].balance);
							fclose(file);
							break;
						case '3':	//withdraw
							bzero(buff, MAX);
							read(connfd, &amount, sizeof(int));

							if(client[SELECTEDUSER].balance > amount){
								client[SELECTEDUSER].balance = client[SELECTEDUSER].balance - amount;

									if(SELECTEDUSER == 0){
										file = fopen(USER1CHAR1,"w");
										fprintf(file,"%d", client[0].balance);
										fclose(file);
									}else{
										file = fopen(USER1CHAR2,"w");
										fprintf(file,"%d",client[1].balance);
										fclose(file);
									}

									file = fopen(LOGCHAR,"a");
									dataclock = localtime(&myclock);
									strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
									fprintf(file,"%s %s wypłacił: %d i posiada: %d\n", mytime,user,amount,client[SELECTEDUSER].balance);
									fclose(file);

									write(connfd,"y", 1);
							}
							else
							{
								write(connfd,"n", 1);
							}

							break;
						case '4':
							printf("Nastąpiło wylogowanie\n");
							transaction = false;
							file = fopen(LOGCHAR,"a");
							dataclock = localtime(&myclock);
							strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);

							fprintf(file,"%s Nastąpiło wylogowanie\n", mytime);	//logging
							fclose(file);
							break;
						default:
							printf("Default");
							counter++;
							if (counter > 4)	//if client disconnected / too many incorrect attempts
								transaction = false;
							break;
					}
					if(!transaction)	//end of transaction
						break;
   	    		}

   	        } else{
   	     	time_t myclock = time(NULL);
			struct tm *dataclock;
			char mytime[1000];

   	        	file = fopen(LOGCHAR,"a");
   	        	dataclock = localtime(&myclock);
				strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
				dataclock = NULL;


   	        	fprintf(file,"%s Przerwana próba logowania\n", mytime);
   	        	fclose(file);
   	        	close(sockfd);
   	        }
   	}
		time_t myclock = time(NULL);
		struct tm *dataclock;
		char mytime[1000];

   	    file = fopen(LOGCHAR,"a");	//logging
		dataclock = localtime(&myclock);
		strftime(mytime,1000,"%Y-%m-%d %H:%M:%S", dataclock);
		dataclock = NULL;

		fprintf(file,"%s Zakończono program\n",mytime);
		fclose(file);
   	}
    exit(0);
}


