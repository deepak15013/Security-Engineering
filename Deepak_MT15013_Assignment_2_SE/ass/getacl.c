#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void showPermission(char*);
void checkPermission(char *);
void showACL(char*);
void checkACLPermission(char*);
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

	if(argc == 2) {
		char file[100];
		strcpy(file,argv[1]);
		if(flagRoot == 1) {
			if(file[0] == '/'){
				char tokenstr[100]="";
				int count=0;
				char* token;
				strcpy(tokenstr,file);
				token = strtok(tokenstr,"/");
				count++;
				while(token != NULL) {
					token = strtok(NULL,"/");
					count++;
				}
				if(count > 4) {
					int len = strlen(file);
					if(file[len-1] == '/') {
						strcat(file, "permission.acl");
					}
					showACL(file);
				}
				else {
					int len = strlen(file);
					if(file[len-1] == '/') {
						strcat(file,"permission.acl");
					}
					else {
						strcat(file,"/permission.acl");
					}
				showPermission(file);
				}
			}
			else {
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				showACL(path);
			}
		}
		else {
			if(file[0] == '/'){
				char tokenstr[100]="";
				int count=0;
				char* token;
				strcpy(tokenstr,file);
				token = strtok(tokenstr,"/");
				count++;
				while(token != NULL) {
					token = strtok(NULL,"/");
					count++;
				}
				if(count > 4) {
					checkACLPermission(file);
					undoSetUid(uid);
					printUid();
					showACL(file);
				}
				else {
					int len = strlen(file);
					if(file[len-1] == '/') {
						strcat(file,"permission.acl");
					}
					else {
						strcat(file,"/permission.acl");
					}
					checkPermission(file);
					undoSetUid(uid);
					printUid();
					showPermission(file);
				}
			}
			else {
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				checkACLPermission(path);
				undoSetUid(uid);
				printUid();
				showACL(path);
			}
		}
	} 
	else if(argc == 1) {
		char file[100];
		strcpy(file,root_dir);
		strcat(file,"/permission.acl");
		if(flagRoot == 1) {		
			showPermission(file);
		}
		else {
			checkPermission(file);
			undoSetUid(uid);
			printUid();
			showPermission(file);
		}

	}
	else {
		printf("Wrong Command\n");
		printf("Usage: ./getacl <path-to-file/path-to-directory/filename/NULL>\n");
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
			printf("Error getting logged-in user\n");
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

void showPermission(char* file) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("Failed to open Permission file\n");
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

void showACL(char* path) {
	char temp_fget[100] = "";
	FILE* fp;
	fp = fopen(path,"r");
	if(fp == NULL) {
		printf("Failed to open ACL File\n");
		exit(1);
	}
	char out[3000] = "";
	while(fgets(temp_fget, sizeof temp_fget, fp)!=NULL) 
    {
    	if((strcmp(temp_fget,"::\n"))==0) {
    		break;
    	}
    	else {
    		strcat(out,temp_fget);
    	}
	}
	printf("%s\n", out);
}