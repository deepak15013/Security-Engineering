#include"stdio.h"    
#include"stdlib.h"    
#include"sys/types.h"    
#include"sys/socket.h"    
#include"string.h"    
#include"netinet/in.h"    
#include"netdb.h"  
     
#define BUF_SIZE 10000   
    
int main(int argc, char**argv) {    
 struct sockaddr_in addr, cl_addr;    
 int sockfd, ret;    
 char buffer[BUF_SIZE];
 char lastMsg[100] = "Enter your username: ";
 struct hostent * server;  
 char * serverAddr;  
 int portno;
  
 if (argc < 3) {  
  printf("usage: client < ip address > <portno>\n");  
  exit(1);    
 }  
  
 serverAddr = argv[1];   
 portno = atoi(argv[2]);
 printf("%d\n", portno);
   
 sockfd = socket(AF_INET, SOCK_STREAM, 0);    
 if (sockfd < 0) {    
  printf("Error creating socket!\n");    
  exit(1);    
 }    
 printf("Socket created...\n");     
 memset(&addr, 0, sizeof(addr));    
 addr.sin_family = AF_INET;    
 addr.sin_addr.s_addr = inet_addr(serverAddr);  
 addr.sin_port = portno;       
  
 ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));    
 if (ret < 0) {    
  printf("Error connecting to the server!\n");    
  exit(1);    
 }    
 printf("Connected to the server...\n");    
  
 memset(buffer, 0, BUF_SIZE);  
 system("clear"); 
 printf("Enter your UserName: ");  
  while(1) {
    fgets(buffer, sizeof(buffer), stdin);
    if(strlen(buffer) >1) {
      strtok(buffer, "\n");
      ret = sendto(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));    
      if (ret < 0) {    
        printf("Error sending data!\n\t-%s", buffer);    
      }  
      ret = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
      strcpy(lastMsg,buffer);
      if (ret < 0) {    
      printf("Error receiving data!\n");      
      } else {    
        fputs(buffer, stdout);    
      }  
    }
    else {
      printf("%s", lastMsg);
    }
    
   
  } 
 return 0;      
}    