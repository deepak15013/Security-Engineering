#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void create(char*, char**);
void checkPermission(char *);
void undoSetUid(int);
void printUid(void);

void main(int argc, char* argv[]) 
{	
	char root_dir[] = "/simple_slash/home/fakeroot";
	int flagRoot = 0;
	int uid = getuid();
	int euid = geteuid();
	printf("Real UID: %d\n",uid);
	printf("Effective UID: %d\n",euid);

	if(uid == euid) 
	{
		flagRoot = 1;
	}

	if(argc >= 2) {
		char file[100];
		strcpy(file,argv[1]);
		if(flagRoot == 1) {
			if(file[0] == '/') {
				create(file, argv);
			}
			else {
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				create(path, argv);
			}
		}
		else {
			if(file[0] == '/') {
				char temp[100]="";
				strcpy(temp,file);
				int len = strlen(temp);
				while(temp[len] != '/') {
					len--;
				}
				temp[len] = '\0';
				strcat(temp,"/permission.acl");
				checkPermission(temp);
				undoSetUid(uid);
				printUid();
				create(file, argv);
				
			}
			else {
				char temp[100] ="";
				strcpy(temp,root_dir);
				strcat(temp,"/permission.acl");
				checkPermission(temp);
				undoSetUid(uid);
				printUid();
				create(file, argv);
			}
		}
		printf("File Written\n");
	} 
	else {
		printf("Wrong Command\n");
		printf("Usage: ./fput <path-to-newfile/filename> <text-to-write-to-file/NULL>\n");
	}
}

void undoSetUid(int uid) {
	int status;

	status = seteuid (uid);
	if (status < 0) {
    	printf("Count't set the Effective UID\n");
 		exit(status);
	}
}

void printUid() {
	int uid = getuid();
	int euid = geteuid();
	printf("Effective UID Changed: %d\n",euid);
}

void checkPermission(char* file) {
	FILE* fp;
	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("Cannot Open\n");
		exit(1);
	}
	else {
		char *user=getenv("USER");
		if(user == NULL) {
			printf("Error getting logged in User\n");
			exit(1);
		}
    	char temp[512];
		int find_result = 0;
        while(fgets(temp, 512, fp) != NULL) 
        {
        	if((strstr(temp, user)) != NULL) 
            {
            	find_result++;
                break;
			}
		}
        if(find_result == 0) {
			printf("Access Denied, Permission Not Granted\n");
			exit(1);
		}
		fclose(fp);
	}
}

void create(char* file, char** argv) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(file,"w+");
	if(fp == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	else {
		//write acl
		char *user = getenv("USER");
		if(user == NULL) {
			printf("error getting username\n");
			exit(1);
		}
		fputs("owner",fp);
		fputs(" ",fp);
		fputs(user,fp);
		fputs(" ",fp);
		fputs("rwx",fp);
		fputs("\n",fp);

		FILE* fpuser;
		fpuser = fopen("/simple_slash/home/users.txt","r");
		if(fpuser == NULL) {
			printf("Error opening file\n");
			exit(1);
		}
		char temp[512];
        while(fgets(temp, 512, fpuser) != NULL) 
        {
        	char *token;
        	token = strtok(temp, "\n");
        	strcpy(temp,token);
        	if((strcmp(temp,user))==0) {
        	}
        	else {
        		fputs("user ",fp);
        		fputs(temp, fp);
        		fputs(" 0",fp);
        		fputs("\n",fp);
        	}
		}
		fputs("other other 0",fp);
		fputs("\n",fp);
		fputs("::",fp);
		fputs("\n",fp);

		//write data
		int i=2;
		while(argv[i] != NULL) {
			fputs(argv[i],fp);
			fputs(" ",fp);
			i++;
		}
		fclose(fpuser);
		fclose(fp);
	}
}