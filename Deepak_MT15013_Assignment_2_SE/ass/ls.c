#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

void show(char*);
void checkPermission(char*);
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
		if(flagRoot == 1) {
			show(argv[1]);
		}
		else {
			char path[100]="";
			strcpy(path,argv[1]);
			int len = strlen(path);
			if((path[len-1]=='/')==1) {
				strcat(path,"permission.acl");
			}
			else {
				strcat(path,"/permission.acl");	
			}
			checkPermission(path);
			undoSetUid(uid);
			printUid();
			show(argv[1]);
		}
	} 
	else if(argc == 1) {
		if(flagRoot == 1) {
			show(root_dir);
		}
		else {
			char path[100]="";
			strcpy(path,root_dir);
			strcat(path,"/permission.acl");
			checkPermission(path);
			strcpy(path,root_dir);
			undoSetUid(uid);
			printUid();
			show(path);
		}
	}
	else {
		printf("Wrong Command\n");
		printf("Usage: ./ls <path-to-directory/NULL>\n");
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

void checkPermission(char* dir) {
	FILE* fp;
	fp = fopen(dir,"r");
	if(fp == NULL) {
		printf("Permission Denied\n");
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

void show(char* dir) {
	char temp_ls[1000] = "";
	DIR *dp;
    struct dirent *direc;
    char temp[100] = "";
	if( NULL == (dp = opendir( dir ))) 
	{
		printf("Directory Empty or Doesn't Exists\n");
	}
    else 
    {
    	while ( direc = readdir(dp) )
		{
        	if( !strcmp(direc->d_name,".") || !strcmp(direc->d_name, "..")) 
            {
            	continue;
            }
            strcpy(temp,direc->d_name);
            if(temp[0] == '.') {
            	continue;
            }
            strcat(temp_ls,direc->d_name);
            strcat(temp_ls,"  ");
        }
        strcat(temp_ls,"\n");
        printf("%s\n", temp_ls);
	}
}