#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void create(char*);
void checkPermission(char *);
int checkUser(char*);
void copyPermissionFile(char*, char*);
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
		if(flagRoot == 1) {
			if(file[0] == '/') {
				create(file);
			}
			else {
				char path[100];
				strcpy(path,root_dir);
				strcat(path,"/");
				strcat(path,file);
				create(path);
			}
		}
		else {
			char* token;
			int userExits=0;
			if(file[0] == '/') {
				char temp[100]="";
				char tokenstr[100]="";
				strcpy(temp,file);
				strcpy(tokenstr,file);

				token = strtok(tokenstr,"/");
				
				while( token != NULL) {
					userExits = checkUser(token);
					if(userExits == 1) {
						break;
					}
					token = strtok(NULL, "/");	
				}
				if(token == NULL) {
					printf("User does not exists\n");
					exit(1);
				}				
				strcpy(temp,"/simple_slash/home/");
				strcat(temp,token);
				strcat(temp,"/permission.acl");
				checkPermission(temp);
				undoSetUid(uid);
				printUid();
				create(file);
				
			}
			else {
				char temp[100] ="";
				strcpy(temp,root_dir);
				strcat(temp,"/permission.acl");
				checkPermission(temp);
				undoSetUid(uid);
				printUid();
				strcpy(temp,root_dir);
				strcat(temp,"/");
				strcat(temp,file);
				create(temp);
			}
		}
	} 
	else {
		printf("Wrong Command\n");
		printf("Usage: ./create_dir <path-to-new-directory>\n");
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

int checkUser(char* token) {
	FILE* fp;
	fp = fopen("/simple_slash/home/users.txt","r");
	if(fp == NULL) {
		printf("Cannot open users file\n");
		exit(1);
	}
	char temp[512];
	int find_result = 0;
    while(fgets(temp, 512, fp) != NULL) 
    {
      	if((strstr(temp, token)) != NULL) 
        {
           	find_result++;
            break;
		}
	}
	fclose(fp);
	return find_result;
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

void create(char* file) {
	char perfile[100];
	char tmp[256];
	char *p = NULL;
    size_t len;
    int check = -1;
    int count = 0;
	snprintf(tmp, sizeof(tmp),"%s",file);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
    	tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
    	if(*p == '/') {

        	*p = 0;
            check = mkdir(tmp, S_IRWXU);
            if(count == 2) {
            	strcpy(perfile,tmp);
            	strcat(perfile,"/permission.acl");
            }
            if(check == 0) {
            	//copy the parent permissions file
            	copyPermissionFile(perfile,tmp);
            }
            
            count++;
            *p = '/';
		}
	check = mkdir(tmp, S_IRWXU);
	if(check == 0) {
		copyPermissionFile(perfile,tmp);
	}
	if(check == 0) {
		printf("Directory Created\n");
	}
	else {
		printf("Cannot Create Directory\n");
	}
}

void copyPermissionFile(char* perfile, char* tmp) {
	FILE *fptr1, *fptr2;
    char filename[100], c;
    strcpy(filename,tmp);
    strcat(filename,"/permission.acl");

    fptr1 = fopen(perfile, "r");
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s \n", filename);
        exit(1);
    }
    fptr2 = fopen(filename, "w");
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s \n", filename);
        exit(1);
    }
    c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
    printf("Permission Copied\n");
    fclose(fptr1);
    fclose(fptr2);
}