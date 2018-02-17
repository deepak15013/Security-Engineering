#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void showPermission(char*);
void checkACLPermission(char *);
void editACL(char*, char*, char*);
void checkUser(char*);
void checkAccess(char*);
void checkOwner(char*, char*);
void undoSetUid(int);
void printUid(void);

void main(int argc, char* argv[]) {
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

	if(argc == 4) {
		char file[100];
		strcpy(file,argv[1]);
		if(flagRoot == 1) {
			if(file[0] == '/'){
				//absolute path
				char* username = argv[2];
				checkUser(username);
				char* access = argv[3];
				checkAccess(access);
				checkOwner(file, username);
				editACL(file, username, access);
			}
			else {
				//Direct File name passed
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				char *username = argv[2];
				checkUser(username);
				char *access = argv[3];
				checkAccess(access);
				checkOwner(file, username);
				editACL(path, username, access);
			}
		}
		else {
			if(file[0] == '/') {
				//absolute path
				char* username = argv[2];
				checkUser(username);
				char* access = argv[3];
				checkAccess(access);
				checkACLPermission(file);
				checkOwner(file, username);
				undoSetUid(uid);
				printUid();
				editACL(file, username, access);
			}
			else {
				//accessing local directory file
				char path[100];
				strcpy(path, root_dir);
				strcat(path,"/");
				strcat(path,file);
				char* username = argv[2];
				checkUser(username);
				char* access = argv[3];
				checkAccess(access);
				checkACLPermission(path);
				checkOwner(path, username);
				undoSetUid(uid);
				printUid();
				editACL(path, username, access);
			}
		}
		printf("Acess Permission Changed\n");
	} 
	else {
		printf("Wrong Command\n");
		printf("Usage: ./setacl <path-to-file> <username> <access-code>\n");
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

void checkAccess(char* access) {
	if((strcmp(access,"rwx"))==0) {
		return;
	}
	else if((strcmp(access,"0"))==0) {
		return;
	}
	else {
		printf("Please give write access codes\n");
		exit(1);
	}
}

void checkUser(char* username) {
	FILE* fp;
	fp = fopen("/simple_slash/home/users.txt","r");
	if(fp == NULL) {
		printf("Cannot Open Users File\n");
		exit(1);
	}
	char temp[512];
	int find_result = 0;
	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp,username)) != NULL) {
			find_result++;
			break;
		}
	}
	if(find_result == 0) {
		printf("No such User Exists.\n");
		exit(1);
	}
	fclose(fp);
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

void showPermission(char* file) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("Failed to open file\n");
		exit(1);
	}
	char out[3000] = "";
	while(fgets(temp_fget, sizeof temp_fget, fp)!=NULL) 
    {
    	strcat(out,temp_fget);
	}
	printf("%s\n", out);
	fclose(fp);
}

void checkOwner(char* file, char* username) {
	char temp[100] = "";
	FILE* fp;
	fp = fopen(file, "r");
	if(fp == NULL) {
		printf("error opening file\n");
		exit(1);
	}
	fgets(temp, 100, fp);
	if((strstr(temp, username)) != NULL) {
		printf("Cannot change file's owner Permissions\n");
		exit(1);
	}
}

void editACL(char* path, char* username, char* access) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(path,"r+");
	if(fp == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	FILE* fp1;
	fp1 = fopen("copy.c","w");
	if(fp1 == NULL) {
		printf("Error creating new file to write\n");
		exit(1);
	}
	while(fgets(temp_fget, sizeof temp_fget, fp)!=NULL) 
    {
    	if((strcmp(temp_fget,"::\n"))==0) {
    		fputs(temp_fget,fp1);
    		while(fgets(temp_fget, sizeof temp_fget, fp) != NULL) {
    			fputs(temp_fget, fp1);
    		}
    		break;
    	}
    	else {
    		if((strstr(temp_fget, username)) != NULL) {
    			char newLine[100]="";
    			strcpy(newLine,"user ");
    			strcat(newLine,username);
    			strcat(newLine," ");
    			strcat(newLine,access);
    			strcat(newLine,"\n");
    			fputs(newLine, fp1);
    		}
    		else {
    			fputs(temp_fget, fp1);
    		}
    	}
	}
	remove(path);
	rename("copy.c",path);
	fclose(fp1);
	fclose(fp);
}