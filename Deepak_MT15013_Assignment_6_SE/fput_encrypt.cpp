#include "functions.h"

using namespace std;

void checkArguments(int argc,char *argv[]);
void executeFputCommand();
void createNewFile(char *fileName);
void appendFile(char *fileName);
void addDataToFile(char *data,FILE *fp);

char newFileName[50];
char *workingDirectory;
static uid_t euid,ruid;
char *fname, *dname;
const char *username;

int main(int argc,char *argv[])
{
	uid_t euid,ruid;
	euid=getEffectiveUserID();
	ruid=getRealUserID();
	int isRootUser=checkUserId(ruid);
	checkArguments(argc,argv);
	username=getUserName();
	if(isRootUser)
		executeFputCommand();
	else
	{
		changeUserId(ruid);//Change the effective user id
		getEffectiveUserID();
		getRealUserID();
		executeFputCommand();//execute Ls command
		restoreUserId(euid);//Restore the effective user id
		getEffectiveUserID();
		getRealUserID();
	}
	return 0;
}


void renameFile(char *fileName)
{

	int i=0;
	while(fileName[i]!='\0'){
		newFileName[i]=fileName[i];
		i++;
	}
	int j=0;
	char extension[]=".acl";
	while(extension[j]!='\0'){
		newFileName[i]=extension[j];
		i++;j++;
	}
	newFileName[i]='\0';
}

void checkArguments(int argc,char *argv[])
{
	if(argc!=2)
	{
		cout<<"Usage : You need to enter the file name as argument...\n";
		exit(-1);
	}
	else
	{
		workingDirectory=argv[1];
	}
}

void executeFputCommand()
{
	char *fileName=getAbsolutePath(workingDirectory);
	splitFileName(&fname,&dname,&fileName);
	int isAccessible=0,isDirExist=0,isFileExist=0;
	isDirExist=checkDirectoryExist(dname);
	Rights right=WRITE;
	if(isDirExist==1)
	{
		isAccessible=checkDirectoryAccess(dname,getUserName(),right);
		cout<<"Accessible : "<<isAccessible;
		if(isAccessible==1)
		{
			isFileExist=checkFileExist(fileName);
			cout<<isFileExist<<"\n";
			if(isFileExist==0)
			{
				createNewFile(fileName);
			}
			else
			{
				appendFile(fileName);
			}
		}
		else
		{
			cout<<"You don't have permissions to write in this directory...\n";
		}
	}
	else
	{
		cout<<"Directory doesn't exists...\n";
		exit(-1);
	}
}


void addPermissionsToFile(char *file)
{	
	int i=0;
	char permissions[]="-rwx-wxr-x\n";
	FILE *fp;
	renameFile(file);
	char *fileName=newFileName;
	cout<<"\n\n"<<fileName;
	fp=fopen(fileName,"w");

	while(username[i]!='\0')
	{
		putc(username[i],fp);
		i++;
	}
	putc(':',fp);
	i=0;
	while(permissions[i]!='\0')
	{		
		putc(permissions[i],fp);
		i++;
	}
	initializeAcls(fp,permissions);
	putc('\n',fp);
	putc('*',fp);
	putc('\n',fp);
	fclose(fp);
	cout<<"\nPermissions added succesfully...\n";
}

void createNewFile(char *fileName)
{
	FILE *fp;
	char *data;
	unsigned char *output;
	fp=fopen(fileName,"w");
	cout<<"\nFile created...\n";
	addPermissionsToFile(fileName);
	cout<<"\nEnter data to be stored : \n";

	getMessageFromConsole(&data);
	cout<<data;
//	callEncryptionFunc(data,&output,fileName);
	aes_init(1,(unsigned char*)data,&output,fileName);
	cout<<"Output received : "<<output;
	addDataToFile((char*)output,fp);
	fclose(fp);
	cout<<"\nData entered succesfully...\n";
}

/*
void addDataToFile(char *data,FILE *fp)
{
	int i=0;
	while(data[i]!='\0')
	{
		putc(data[i],fp);
		i++;
	}
}
*/

void appendDataToFile(char *data,char *fileName)
{
	FILE *fp=NULL;
	unsigned char *output,*cipherText;
	char *bufferSent;
	//Decrypt the file
	readData(fileName,&bufferSent);
	aes_init(2,(unsigned char*)bufferSent,&output,fileName);
//	callDecryptionFunc(bufferSent,&output);

	//Appending the file contents
	int length=BUFFER_LENGTH-1;
	char *newBuffer=(char*)malloc(sizeof(char)*BUFFER_LENGTH);
	int position=0;
	int i=0;
	while(output[i]!='\0')
	{
		cout<<"*";
		newBuffer[position]=output[i];
		position++;i++;
		if(position>length)
		{
			length=length+64;
			newBuffer=(char*)realloc(newBuffer,length*sizeof(char));
			if(newBuffer==NULL)
			{
				cout<<"Error in reallocation...\n";
				exit(-1);
			}
		}
	}
	i=0;
	while(data[i]!='\0')
	{
		newBuffer[position]=data[i];
		position++;i++;
		if(position>length)
		{
			length=length+64;
			newBuffer=(char*)realloc(newBuffer,length*sizeof(char));
			if(newBuffer==NULL)
			{
				cout<<"Error in reallocation...\n";
				exit(-1);
			}
		}
	}
	newBuffer[position]='\0';
	fp=fopen(fileName,"w");
	if(fp!=NULL)
	{
		//callEncryptionFunc(newBuffer,&cipherText,fileName);
		aes_init(1,(unsigned char*)newBuffer,&cipherText,fileName);
		cout<<"Output received : "<<cipherText;
		addDataToFile((char*)cipherText,fp);
		fclose(fp);
	}
	else
	{
		cout<<"Error in opening file...\n";
		exit(-1);
	}
}

void appendFile(char *file)
{
	char *data=NULL;
	int isAllowedToAccess=0;
	char *filePermissions;
	cout<<"Inside append file...\n";
	ACL *acl;
	acl=(ACL*)malloc(sizeof(ACL));

	renameFile(file);
	char *fileName=newFileName;
	cout<<fileName<<"\n";
	*acl=getPermissionsFromFile(fileName,&filePermissions);
	cout<<"Permissions received\n";	
	isAllowedToAccess=checkWritePermission(filePermissions,username,acl);
	printf("Access : %d\n",isAllowedToAccess);
	if(isAllowedToAccess==1)
	{
		cout<<"Enter data to be stored : \n";
		getMessageFromConsole(&data);
		cout<<"Data entered..."<<data<<"\n";
		const char *substring="%n";
		if(!strstr(data,substring))
		{
			appendDataToFile(data,file);
		}
		else
		{
			cout<<"Enter data in proper format...\n";
			exit(-1);
		}
		cout<<"\nData entered successfully...\n";
	}
	else
		cout<<"You don't have permissions to write in this file...\n";
}

