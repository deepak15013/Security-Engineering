#include "functions.h"

using namespace std;


void addPermissionsInACL(unsigned char *user);
void initializeAcls(char *fileName);
void readFileForInitialACLValue(char *fileName);
void getUserPermissions();
void readAllUsersFromFile(ACL *acl);
void addACLToFile(ACL *acl,FILE *fp);

char permissions[20];
char userId[10];
char userPermission[4];
int readFileOfUser();
int noOfLinesInFile();
void readACLFromFile(char *fileName);

int noOfLinesInFile()
{
	int noOfLines=0;
	string line;
	ifstream myfile("/simple_slash/myetc/users.txt");
	while (getline(myfile,line))
		++noOfLines;
	cout << "Number of lines in text file: " <<noOfLines<<"\n";
	return noOfLines;
}

void copyPermissions(char *permissionsReceived)
{
	int i=0;
	while(permissionsReceived[i]!='\0')
	{
		permissions[i]=permissionsReceived[i];
		i++;
	}
	permissions[i]='\0';
}

void initializeAcls(FILE *fp,char *permissionsReceived)
{
	ACL acl;
	copyPermissions(permissionsReceived);
	getUserPermissions();
	readAllUsersFromFile(&acl);
	addACLToFile(&acl,fp);
}

void getUserPermissions()
{
	userPermission[0]=permissions[1];
	userPermission[1]=permissions[2];
	userPermission[2]=permissions[3];
	userPermission[3]='\0';
}



void readAllUsersFromFile(ACL *acl)
{
	string line;
	char user[20];
	int fileLength=noOfLinesInFile();
	acl->aclUsers=(unsigned char**)malloc(sizeof(unsigned char*)*fileLength);
	acl->aclLen=(unsigned int)fileLength;
	FILE *fp=fopen("/simple_slash/myetc/users.txt","r");
	for(int i=0;i<fileLength;i++)
	{
		acl->aclUsers[i]=(unsigned char*)malloc(sizeof(unsigned char)*20);
		fgets((char *)(acl->aclUsers[i]),20,fp);
		addPermissionsInACL(acl->aclUsers[i]);
		cout<<"Users : "<<acl->aclUsers[i]<<"\n";
	}
	fclose(fp);
}

void addACLToFile(ACL *acl,FILE *fp)
{	
	if(fp!=NULL){
		fseek(fp, 0, SEEK_END);
		int i=0;
		fprintf(fp,"%d\n",acl->aclLen);
		while(i<acl->aclLen)
		{
			fprintf(fp,"%s",acl->aclUsers[i]);
			putc('\n',fp);
			i++;
		}
		cout<<"ACLs are written into file...\n";
		
		//fclose(fp);
	}
	else{
		cout<<"Error in writing ACLS...\n";
		exit(-1);
	}
	//readACLFromFile(fileName);
}

void updateFileACL(char *fileName,ACL *acl,char *fileData,char *filePermissions)
{
	FILE *fp=fopen(fileName,"w");
	if(fp!=NULL){
		int i=0;
		while(filePermissions[i]!='\0')
		{		
			putc(filePermissions[i],fp);
			i++;
		}
		putc('\n',fp);
		addACLToFile(acl,fp);
		putc('\n',fp);
		putc('*',fp);
		putc('\n',fp);
		i=0;
		while(fileData[i]!='\0')
		{
			putc(fileData[i],fp);
			i++;
		}
		fclose(fp);
	}
	else{
		cout<<"Error in writing ACLS...\n";
		exit(-1);
	}
	//readACLFromFile(fileName);
}


void mergeStrings(char *u1,char *u2,char *mergedString)
{
	int i=0,j=0;
	while(u1[i]!='\0')
	{
		mergedString[j]=u1[i];
		i++;j++;
	}
	mergedString[j]=':';
	j++;i=0;
	while(u2[i]!='\0')
	{
		mergedString[j]=u2[i];
		i++;j++;
	}
	mergedString[j]='\0';
}

void copyString(unsigned char *s1,unsigned char *s2)
{
	int i=0;
	while(s2[i]!='\0')
	{
		s1[i]=s2[i];
		i++;
	}
	s1[i]='\0';
}

ACL updateACLFromFile(char *fileName,char *potentialUser,char *changedPermission)
{
	ACL *acl;
	char **userArray;
	acl=(ACL*)malloc(sizeof(ACL));
	char ch;
	FILE *fp=fopen(fileName,"r+");
	if(fp!=NULL)
	{
		while((ch=getc(fp))!='\n');

		fscanf(fp,"%u",&acl->aclLen);
		int i=0;
		acl->aclUsers=(unsigned char**)malloc(sizeof(unsigned char*)*acl->aclLen);
		cout<<"Inside update func\n";
		while(i<acl->aclLen)
		{
			acl->aclUsers[i]=(unsigned char*)malloc(sizeof(unsigned char)*20);			
			fscanf(fp,"%s",(acl->aclUsers[i]));
			userArray=shellSplitLine((char*)acl->aclUsers[i],0);
			if(compareString(userArray[0],potentialUser)==1)
			{
				userArray[1]=changedPermission;
			}

			mergeStrings(userArray[0],userArray[1],userArray[0]);
			copyString(acl->aclUsers[i],(unsigned char*)userArray[0]);
			i++;
		}		
		fclose(fp);
		for(int i=0;i<acl->aclLen;i++)
		{
			cout<<"User : "<<acl->aclUsers[i]<<"\n";
		}
	}
	return *acl;
}

void readACLFromFile(char *fileName)
{
	ACL *acl;
	acl=(ACL*)malloc(sizeof(ACL));
	char ch;
	FILE *fp=fopen(fileName,"r");
	if(fp!=NULL)
	{

		while((ch=getc(fp))!='\n');

		fscanf(fp,"%u",&acl->aclLen);
		int i=0;
		acl->aclUsers=(unsigned char**)malloc(sizeof(unsigned char*)*acl->aclLen);
		while(i<acl->aclLen)
		{
			acl->aclUsers[i]=(unsigned char*)malloc(sizeof(unsigned char)*20);			
			fscanf(fp,"%s",(acl->aclUsers[i]));
			i++;
		}
		
		fclose(fp);
	}
	cout<<"\nACL Values : \n";
	cout<<"Length of ACLS : "<<acl->aclLen<<"\n";

	for(int i=0;i<acl->aclLen;i++)
	{
		//cout<<"*";
		cout<<"User : "<<acl->aclUsers[i]<<"\n";
	}
}

void addPermissionsInACL(unsigned char *user)
{
	int i=0,j=0;
	while(user[i]!='\0')i++;
	user[--i]=':';
	i++;
	while(userPermission[j]!='\0')
	{
		user[i]=userPermission[j];
		i++;j++;
	}
	user[i]='\0';
}


//Opening and reading file containing users in the file system
int readFileOfUser()
{
	char ch;
	FILE *filePointer;
	char user[20];
	char *lineInFile=NULL;
	size_t length=0;
	ssize_t read;
	char ** tokenArray;
	int temp=0;
	int flag=0,i;
	filePointer=fopen("/simple_slash/myetc/users.txt","r");
	if(filePointer==NULL){
		printf("Error in opening Users file\n");exit(-1);
	}
	temp=strlen(userId);

	printf("\nuserIds : %s %d\n",userId,temp);
	i=0;temp=0;
	while((ch=getc(filePointer))!=EOF)
	{
		i=0;
		while(ch!='\n' && ch!=EOF)
		{
			user[i]=ch;
			i++;
			ch=getc(filePointer);
		}
		user[i]='\0';
		temp=0;
		temp=compareString(user,userId);
		printf("Compare : %s %s %d\n",user,userId,temp);
		if(temp==1)
		{
			flag=1;
			break;
		}
		if(ch==EOF)
			break;
	}
	fclose(filePointer);
	if(flag==1)
		return 1;
	else
		return 0;
}

