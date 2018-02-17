#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void show(char*);
void checkACLPermission(char*);
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

	if(argc == 2) {
		char file[100];
		strcpy(file,argv[1]);
		if((strcmp(file,"permission.acl"))==0) {
			printf("Cannot Access Permission File\n");
			printf("Use Command: <./getacl> to Access the Permission File\n");
			exit(1);
		}
		if(flagRoot == 1) {
			if(file[0] == '/') {
				show(file);
			}
			else {
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				show(path);
			}
		}
		else {
			if(file[0] == '/') {
				checkACLPermission(file);
				undoSetUid(uid);
				printUid();
				show(file);
			}
			else {
				char temp[100] = "";
				strcpy(temp,root_dir);
				strcat(temp,"/");
				strcat(temp,file);
				checkACLPermission(temp);
				undoSetUid(uid);
				printUid();
				show(temp);
			}
		}
	} 
	else {
		printf("Wrong Command\n");
		printf("Usage: ./fget <path-to-file>\n");
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

void checkACLPermission(char* file) {
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
        	if((strcmp(temp,"::\n"))==0) {
        		break;
        	}
        	if((strstr(temp, user)) != NULL) 
            {
            	if((strstr(temp,"rwx")) != NULL) {
            		find_result++;
            	}
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

void show(char* file) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	char out[3000] = "";
	while(fgets(temp_fget, sizeof temp_fget, fp)!=NULL) 
    {
    	if((strcmp(temp_fget,"::\n"))==0) {
    		break;
    	}
	}
	while(fgets(temp_fget, sizeof temp_fget, fp)!=NULL) 
    {
    	strcat(out,temp_fget);
	}
	int print = printf("%s", out);
	if(print == 0) {
		printf("Please Specify a File Name\n");
		exit(1);
	}
	printf("\n");
}