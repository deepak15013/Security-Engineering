#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void edit(char*, char**);
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

	if(argc == 3 || argc == 4) {
		char file[100];
		if(flagRoot == 1) {
			if(argc == 4) {
				strcpy(file,argv[3]);
				int len = strlen(file);
				if(file[len-1] == '/') {
					strcat(file,"permission.acl");
				}
				else {
					strcat(file, "/permission.acl");
				}
				edit(file, argv);
			}
			else {
				strcpy(file,root_dir);
				strcat(file,"/permission.acl");
				edit(file,argv);
			}
		}
		else {
			if(argc == 4) {
				strcpy(file,argv[3]);
				int len= strlen(file);
				if(file[len-1] == '/') {
					strcat(file,"permission.acl");
				}
				else {
					strcat(file, "/permission.acl");
				}
				checkPermission(file);
				undoSetUid(uid);
				printUid();
				edit(file,argv);
			}
			else {
				strcpy(file,root_dir);
				strcat(file,"/permission.acl");
				checkPermission(file);
				undoSetUid(uid);
				printUid();
				edit(file,argv);
			}
		}
	} 
	else {
		printf("Wrong Command\n");
		printf("Usage: ./chmod <access-code> <username> <path-to-directory/NULL>\n");
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
	printf("TEffective UID Changed: %d\n",euid);
}

void checkPermission(char* file) {
	FILE* fp;
	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("Cannot Open Permission File\n");
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

void edit(char* file, char** argv) {
	int find_result = 0;
	char temp_fget[100] = "";
	if((strcmp(argv[1],"0"))==0) {
		FILE* fptr;
		fptr = fopen(file,"a+");
		if(fptr == NULL) {
			printf("cannot open permission file\n");
			exit(1);
		}
		int line = 0;
		while(fgets(temp_fget, 20, fptr)!=NULL) 
    	{
    		if((strstr(temp_fget, argv[2])) != NULL) 
            {
            	find_result++;
                break;
			}
			line++;
		}
		if(find_result > 0) {
			FILE *fp1, *fp2;
        	int c;
        	int del_line= ++line, temp = 1;

        	fp1 = fopen(file, "r");
        	c = fgetc(fp1);
        	while (c != -1 )
        	{
          		c = fgetc(fp1);
        	}
        	rewind(fp1);
        	fp2 = fopen("copy.c", "w");
	        c = getc(fp1);
    	    if(c == EOF) {
        		printf("empty c\n");
        		exit(1);
        	}
        	rewind(fp1);
        	while (c != EOF) {
          		c = getc(fp1);
          		if (c == '\n')
		          temp++;
        		if (temp != del_line)
          		{
            		putc(c, fp2);
		        }
        	}
        	fclose(fp1);
        	fclose(fp2);
        	remove(file);
        	rename("copy.c", file);
        	FILE* fp3;
        	fp3 = fopen(file,"a+");
        	fseeko(fp3,-1,SEEK_END);
        	int position = ftello(fp3);
        	ftruncate(fileno(fp3), position);
        	fclose(fp3);
        	printf("Access Removed\n");
		}
		else {
			printf("User does not exists\n");
		}
		fclose(fptr);
	}
	else if((strcmp(argv[1],"rwx"))==0) {
		FILE* fp;
		fp = fopen(file, "a+");
		if(fp == NULL) {
			printf("Cannot open permission file\n");
			exit(1);
		}
		while(fgets(temp_fget, 20, fp)!=NULL) 
    	{
    		if((strstr(temp_fget, argv[2])) != NULL) 
            {
            	find_result++;
                break;
			}
		}
		if(find_result > 0) {
			printf("User already has Access\n");
			exit(1);
		}
		else {
			fprintf(fp, "%s rwx\n", argv[2]);
		}
		fclose(fp);
		printf("Access Granted\n");
	}
	else {
		printf("wrong command\n");
		exit(1);
	}
	
}