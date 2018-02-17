#include <stdio.h>  
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <netinet/in.h>  
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
  
#define BUF_SIZE 10000  
#define CLADDR_LEN 100 

  
int main(int argc, char *argv[]) 
{  
  struct sockaddr_in addr, cl_addr;  
  int sockfd, len, ret, newsockfd;  
  char buffer[BUF_SIZE];  
  pid_t childpid;  
  char clientAddr[CLADDR_LEN];  
  int portno;
  int i=0;
  int flag=0;
  char path[] = "@deepakpc:~/simple_slash/simple_home/";
  int k,j;
  char send[BUF_SIZE];
  char *sendptr = send;
  char dir[100];


  if(argc < 2) 
  {
    printf("No Port Provided\n");
    exit(1);
  }

   
  sockfd = socket(AF_INET, SOCK_STREAM, 0);  
  if (sockfd < 0) 
  {  
    printf("Error creating socket!\n");  
    exit(1);  
  }  
  printf("Socket created...\n");  
   
  printf("1:%d\n",portno);
  portno = atoi(argv[1]);
  printf("2:%d\n", portno);

  memset(&addr, 0, sizeof(addr));  
  addr.sin_family = AF_INET;  
  addr.sin_addr.s_addr = INADDR_ANY;  
  addr.sin_port = portno;  
   
  ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));  
  if (ret < 0) 
  {  
    printf("Error binding!\n");
    exit(1);  
  }  
  printf("Binding done...\n"); 
  system("clear");
  
  printf("Waiting for a connection...\n");  
  listen(sockfd, 50);  

  struct stat st = {0};

  if (stat("./simple_slash", &st) == -1) {
    mkdir("./simple_slash", 0700);
    printf("Root created\n");
  }
  else {
    printf("Root directory already Exits\n");
  }

  FILE* fp_create;
  fp_create = fopen("./simple_slash/users.txt","w+");
  if(fp_create == NULL) {
    printf("File not created\n");
    exit(1);
  }
  else {
    fputs("u1 g1",fp_create);
    fputs("\n",fp_create);
    fputs("u2 g1",fp_create);
    fputs("\n",fp_create);
    fputs("u3 g2",fp_create);
    fputs("\n",fp_create);
    fputs("u4 g2",fp_create);
    fputs("\n",fp_create);
    fputs("u5 g2",fp_create);
    fputs("\n",fp_create);
    fputs("u6 g2",fp_create);
    fputs("\n",fp_create);
    fputs("u7 g3",fp_create);
    fputs("\n",fp_create);
    fputs("u8 g3",fp_create);
    fputs("\n",fp_create);
  }

  if(fp_create) {
    fclose(fp_create);
  }

  if (stat("./simple_slash/simple_home", &st) == -1) {
    mkdir("./simple_slash/simple_home", 0700);
    printf("Home created\n");
  }
  else {
    printf("Home directory already Exits\n");
  }


  
  for (;;) 
  { //infinite loop  
    char user[2];
    len = sizeof(cl_addr);  
    newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);  
    if (newsockfd < 0) 
    {  
      printf("Error accepting connection!\n");  
      exit(1);  
    }  
    printf("Connection accepted...\n");  
  
    inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);  
    if ((childpid = fork()) == 0) 
    { //creating a child process  
  
      close(sockfd);  
   //free(sockfd); 
//stop listening for new connections by the main process.   
//the child will continue to listen.   
//the main process now handles the connected client.  
  
      for (;;) 
      {  
        memset(buffer, 0, BUF_SIZE);  
        ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);  
        buffer[ret] = '\0';
        if(ret < 0) 
        {  
          printf("Error receiving data!\n");    
          exit(1);  
        }    
        printf("Received data from %s: %s\n", clientAddr, buffer); 

        if(buffer[0] == 'u' && strlen(buffer)>1 && flag==0) {
          FILE *fp;
          char temp[512];
          char* str = buffer;
          int result = 0;
          strcpy(user, buffer);
          fp = fopen("./simple_slash/users.txt","r");
          if(fp == NULL) {
            printf("Not able to open user list files\n");
            exit(1);
          }

          while(fgets(temp,512,fp)!= NULL) {
            if((strstr(temp,str)) != NULL) {
                result++;
                break;
            }
          }

          if(result == 0) {
              printf("Wrong username\n");
              char send[100] = "user does not exists";
              strcat(send,"\n");
              strcat(send,"Enter your username: ");
              sendptr = send;  
          }
          else {
            flag = 1;
            printf("Auth success\n");

            strcpy(dir,"./simple_slash/simple_home/");

            for(j=0; dir[j]!='\0'; ++j);  /* i contains length of string s1. */
              for(k=0; k<2; ++k, ++j)
              {
                dir[j]=buffer[k];
              }
              dir[j]='\0';
            printf("%s\n", dir);

            char temporary[50] = "";
              strcpy(temporary,buffer);
              strcat(temporary,path);
              strcpy(path,temporary);

            if (stat(dir, &st) == -1) {
              mkdir(dir, 0700);
              printf("Directory created for user %s\n",buffer);
              
              FILE *fp_per;
              char path_per[100];
              strcpy(path_per,dir);
              strcat(path_per,"/permission.txt");
              fp_per = fopen(path_per,"w+");
                  if(fp_per == NULL) {
                    printf("Error Opening file\n");
                    exit(1);
                  }
                  else {
                    printf("permission file created\n");
                    fputs(user,fp_per);
                    fputs(" ",fp_per);
                    fputs("rwx",fp_per);
                  }
                  if(fp_per) {
                    fclose(fp_per);
                  }
            }
            else {
              printf("Directory exits\n");
            }
            strcat(path,buffer);
            strcat(path,"/$");
            sendptr = path;
          }

          if(fp) {
            fclose(fp);
          }

        }
        else {
          if(flag==1) {
            sendptr = path;
            const char s[2] = " ";
            char access_user[2] = "";
            char *arg;
            char *string;
            arg = strtok(buffer, s);


            strcpy(buffer,arg);
            arg = strtok(NULL, s);
            string = arg;

            if(arg != NULL) {
                int len = strlen(arg);

                if((arg[len-1] == '/') == 1) {
                  access_user[0] = arg[len-3];
                  access_user[1] = arg[len-2];
                }
                else {
                  access_user[0] = arg[len-2];
                  access_user[1] = arg[len-1];
                }  
            }

            string = strtok(NULL, s);

            if((strcmp(buffer, "ls"))==0) {
              // printf("ls Execute\n");
              
              char temp_ls[1000] = "";
              if(arg == NULL) {
                DIR *dp;
                struct dirent *direc;
                struct stat fdetails;
                if( NULL == (dp = opendir( dir ))) 
                {
                  // printf("Empty\n");
                }
                else 
                {
                  while ( direc = readdir(dp) )
                  {
                    if( !strcmp(direc->d_name,".") || !strcmp(direc->d_name, "..")) 
                    {
                      continue;
                    }
                    strcat(temp_ls,direc->d_name);
                    strcat(temp_ls,"\t");
                  }
                  strcat(temp_ls,"\n");
                  strcat(temp_ls,path);
                  sendptr = temp_ls;
                }
              }
              else 
              {
                // printf("with arg\n");
                char file_path[100] = ".";
                strcat(file_path,arg);
                int len = strlen(file_path);
                if((file_path[len-1] == '/')==1) {
                  strcat(file_path,"permission.txt");
                }
                else {
                  strcat(file_path,"/permission.txt");
                }

                FILE *file_ptr;
                
                file_ptr = fopen(file_path,"r");
                if(file_ptr == NULL) {
                  printf("Cannot open file\n");
                }
                else {
                  // printf("opened file\n");
                  char temp[512];
                  int find_result = 0;
                  while(fgets(temp, 512, file_ptr) != NULL) 
                  {
                    if((strstr(temp, user)) != NULL) 
                    {
                      // printf("%s\n", temp);
                      find_result++;
                      break;
                    }
                  }

                if(find_result == 0) {
                  // printf("Access Denied for reading directory\n");
                  char str[500] = "Access denied, not enough permission";
                  strcat(str,"\n");
                  strcat(str,sendptr);
                  sendptr = str;
                }
                else {
                  // printf("Access granted to read\n");

                  char rel_path[100] = "./";
                  strcat(rel_path,arg);
                  DIR *dp;
                  struct dirent *direc;
                  struct stat fdetails;
                  if( NULL == (dp = opendir( rel_path ))) 
                  {
                    // printf("Empty\n");
                  }
                  else 
                  {
                    while ( direc = readdir(dp) )
                    {
                      if( !strcmp(direc->d_name,".") || !strcmp(direc->d_name, "..")) 
                      {
                        continue;
                      }
                      strcat(temp_ls,direc->d_name);
                      strcat(temp_ls,"\t");
                    }
                    strcat(temp_ls,"\n");
                    strcat(temp_ls,path);
                    sendptr = temp_ls;
                  }
                }
                if(file_ptr) {
                  fclose(file_ptr);
                }
              }
            }
          }
            else if((strcmp(buffer, "fput"))==0) {
              // printf("Execute fput\n");
              char temp_fput[2000] = "";
              int fput_flag = 0;
              if(arg != NULL) {




                FILE *fp_fput;
                if(arg[0] == '/') {



                  char str[100] = ".";
                  strcat(str,arg);
                  int last = strlen(str);

                  if(str[last-1] == '/') {
                    str[last-1] = '\0';
                  }

                  char file_path[100];
                  strcpy(file_path,str);

                  while(file_path[last] != '/') {
                    --last;
                  }
                  file_path[last] = '\0';
                  strcat(file_path,"/permission.txt");

                  FILE *file_ptr;
                
                  file_ptr = fopen(file_path,"r");
                  if(file_ptr == NULL) {
                    // printf("Cannot open file\n");
                  }
                  else {
                    // printf("opened file\n");
                    char temp[512];
                    int find_result = 0;
                    while(fgets(temp, 512, file_ptr) != NULL) 
                    {
                      if((strstr(temp, user)) != NULL) 
                      {
                        // printf("%s\n", temp);
                        find_result++;
                        break;
                      }
                    }

                  if(find_result == 0) {
                    fput_flag = 1;
                    // printf("Access Denied for reading directory\n");
                    char str[500] = "Access denied, not enough permission";
                    strcat(str,"\n");
                    strcat(str,sendptr);
                    sendptr = str;
                  }
                  else {
                    fput_flag = 1;
                    // printf("Access granted to read\n");

                    fp_fput = fopen(str,"w+");
                   if(fp_fput == NULL) {
                      // printf("Error Opening file\n");
                    }
                    else 
                    {
                      while(string != NULL) {
                        fputs(string, fp_fput);
                        fputs(" ",fp_fput);
                        string = strtok(NULL, s);
                      }

                      
                      if(fp_fput) {
                        fclose(fp_fput);
                      }
                    }
                  }

                  if(file_ptr) {
                    fclose(file_ptr);
                  }
                }
              }
                else {
                  char str[100] = "./simple_slash/simple_home/";
                  strcat(str,user);
                  strcat(str,"/");
                  strcat(str,arg);
                  fp_fput = fopen(str,"w+");
                  if(fp_fput == NULL) {
                    // printf("Error Opening file\n");
                  }
                  else {

                    while(string != NULL) {
                      fputs(string, fp_fput);
                      fputs(" ",fp_fput);
                      string = strtok(NULL, s);
                    }

                    fput_flag = 1;
                    if(fp_fput) {
                      fclose(fp_fput);
                    }
                  }
                }

                if(fput_flag == 0) {
                  strcat(temp_fput,"Please provide correct path\n");
                  strcat(temp_fput,path);
                  sendptr = temp_fput;
                }
              }
              else {
                strcat(temp_fput,"missing operand");
                strcat(temp_fput,"\n");
                strcat(temp_fput,"usage: fput <filename/path> <string>\n");
                strcat(temp_fput,path);
                sendptr = temp_fput;
              }
              
            }
            else if((strcmp(buffer, "fget"))==0) 
            {
             // printf("Execute fget\n");
             char temp_fget[3000] = "";
             int fget_flag = 0;

             if(arg != NULL) {
              if(arg[0] == '/') {

              int arg_len = strlen(arg);
              if(arg[arg_len-1]=='/') {
                arg[arg_len-1] = '\0';
              }

              char file_path[100] = "./";
                strcat(file_path,arg);

                  int last = strlen(file_path);
                  while(file_path[last] != '/') {
                    --last;
                  }
                  file_path[last] = '\0';
                  strcat(file_path,"/permission.txt");
                // printf("user:%s\n", user);

                FILE *file_ptr;
                file_ptr = fopen(file_path,"r");
                if(file_ptr == NULL) {
                  // printf("path not found\n");
                }
                else {
                  char temp[512];
                  int find_result = 0;
                  while(fgets(temp, 512, file_ptr) != NULL) 
                  {
                    if((strstr(temp, user)) != NULL) 
                    {
                      // printf("%s\n", temp);
                      find_result++;
                      break;
                    }
                  }

                if(find_result == 0) {
                  // printf("Access Denied for reading directory\n");
                  char str[500] = "Access denied, not enough permission";
                  strcat(str,"\n");
                  strcat(str,sendptr);
                  sendptr = str;
                  fget_flag = 1;
                }
                else {
                  // printf("Access granted to read\n");


                FILE *fp_fget;
                if(arg[0] == '/') {
                  char str[100] = "./"; 
                  strcat(str,arg);
                  fp_fget = fopen(str,"r");
                  if(fp_fget == NULL) {
                    // printf("Error Opening file\n");
                  }
                  else 
                  {
                    char out[3000] = "";
                    while(fgets(temp_fget, sizeof temp_fget, fp_fget)!=NULL) 
                    {
                      strcat(out,temp_fget);
                      
                      
                    }
                      strcat(out,"\n");
                      strcat(out,path);
                      sendptr = out;
                      fget_flag = 1;
                    if(fp_fget) 
                    {
                      fclose(fp_fget);
                    }
                  }

                }
                
                

                  
                }
                if(file_ptr) {
                  fclose(file_ptr);
                }
              }
            }
            else {
                  char str[100] = "./simple_slash/simple_home/";
                  strcat(str,user);
                  strcat(str,"/");
                  strcat(str,arg);
                  FILE *fp_fget;
                  fp_fget = fopen(str,"r");
                  if(fp_fget == NULL) {
                    // printf("Error Opening file\n");
                  }
                  else 
                  {
                    char out[3000]="";
                    while(fgets(temp_fget,sizeof temp_fget, fp_fget)!=NULL) 
                    {
                      strcat(out,temp_fget);
                    }
                    strcat(out,"\n");
                    strcat(out,path);
                    sendptr = out;
                    fget_flag = 1;

                    if(fp_fget) 
                    {
                      fclose(fp_fget);
                    }
                  }
                }
             }
             else 
              {
                // printf("Provide FileName to open\n");
                strcat(temp_fget,"missing operand");
                strcat(temp_fget,"\n");
                strcat(temp_fget,"usage: fget <filename/path>\n");
                strcat(temp_fget,path);
                sendptr = temp_fget;
                fget_flag = 1;
              }

              if(fget_flag == 0) {
                  strcpy(temp_fget,"Please provide correct path\n");
                  strcat(temp_fget,path);
                  sendptr = temp_fget;
                }


            }
             else if((strcmp(buffer, "create_dir"))==0 ) {
              // printf("Execute create_dir\n");
              if(arg != NULL ) {
                if(arg[0] == '/') {
                  char str[80] = ".";                 
                  strcat(str,arg);
                  char check[100]="";
                  strcpy(check,str);

                  int len = strlen(check);
                  if(check[len-1] == '/') {
                    check[len-1] = '\0';
                  }

                  int last = strlen(check);
                  while(check[last] != '/') {
                    --last;
                  }
                  check[last] = '\0';

                  strcat(check,"/permission.txt");
                  FILE *fp_dir;
                  fp_dir = fopen(check,"r");
                  if(fp_dir == NULL) {
                    // printf("Cannot open file\n");
                  }
                  else {
                    // printf("file Opened\n");  
                    char temp[512];
                    int find_result = 0;
                    while(fgets(temp, 512, fp_dir) != NULL) 
                    {
                      if((strstr(temp, user)) != NULL) 
                      {
                        // printf("%s\n", temp);
                        find_result++;
                        break;
                      }
                    }

                  if(find_result == 0) {
                    // printf("Access Denied for reading directory\n");
                    char str[500] = "Access denied, not enough permission";
                    strcat(str,"\n");
                    strcat(str,sendptr);
                    sendptr = str;
                  }
                  else {
                    // printf("Access granted to read\n");
                    if (stat(str, &st) == -1) 
                    {
                      mkdir(str, 0700);
                      char string[100];
                      strcpy(string,"Directory created\n");
                      strcat(string,sendptr);
                      sendptr = string;
                    }
                  }
                }
              }
                else {
                  char str[80] = "./simple_slash/simple_home/";
                  strcat(str,user);
                  strcat(str,"/");
                  strcat(str,arg);

                  if (stat(str, &st) == -1) {
                      mkdir(str, 0700);
                      char string[100];
                      strcpy(string,"Directory created\n");
                      strcat(string,sendptr);
                      sendptr = string;
                  }
                }
              }
              else {
                // printf("argument not present\n");
                char send[100]="";
                strcpy(send,"missing operand");
                strcat(send,"\n");
                strcat(send,"usage: create_dir <directory_name>");
                strcat(send,"\n");
                strcat(send,sendptr);
                sendptr = send;
              }

              
            }
            else {
              char temp[5000];
              strcpy(temp,"Enter a valid Command\n");
              strcat(temp,"Commands: ls, fget, fput, create_dir\n");
              strcat(temp,path);
              sendptr = temp;
            }
          }
          else {
            char temp[] = "Enter your username: ";
            sendptr = temp;
          }
        }
        ret = sendto(newsockfd, sendptr, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
        if (ret < 0) {    
          printf("Error sending data!- %s\n", sendptr);
          exit(1);           
        } 
        printf("Sent data to %s: %s\n", clientAddr, sendptr); 
      }  
    }  
    close(newsockfd);   
  }  
}
