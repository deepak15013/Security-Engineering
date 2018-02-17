#include "functions.h"

using namespace std;

void checkArguments(int argc,char *argv[]);
void executeFputValidate();
void validateFileData(char *fileName);

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
		executeFputValidate();
	else
	{
		changeUserId(ruid);//Change the effective user id
		getEffectiveUserID();
		getRealUserID();
		executeFputValidate();//execute Ls command
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

void executeFputValidate()
{
	char *fileName=getAbsolutePath(workingDirectory);
	splitFileName(&fname,&dname,&fileName);
	int isAccessible=0,isDirExist=0,isFileExist=0;
	isDirExist=checkDirectoryExist(dname);
	Rights right=READ;
	if(isDirExist==1)
	{
		isAccessible=checkDirectoryAccess(dname,getUserName(),right);
		if(isAccessible==1)
		{
			isFileExist=checkFileExist(fileName);
			if(isFileExist==0)
			{
				cout<<"The requested file doesn't exist...\n";
			}
			else
			{
				validateFileData(fileName);
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


void validateFileData(char *file)
{
	char *data;
	int isAllowedToAccess=0;
	char *filePermissions;
	ACL *acl;
	unsigned char *output;
	renameFile(file);
	char *fileName=newFileName;
	cout<<fileName<<"\n";

	acl=(ACL*)malloc(sizeof(ACL));
	getPermissionsFromFile(fileName,&filePermissions);

	isAllowedToAccess=checkReadPermission(filePermissions,username,acl);
	printf("Access : %d\n",isAllowedToAccess);
	char *bufferSent;
	if(isAllowedToAccess==1)
	{
		readData(file,&bufferSent);
		aes_init(2,(unsigned char*)bufferSent,&output,file);
		//callDecryptionFunc(bufferSent,&output);
		cout<<"Data stored in this file is : \n";
		cout<<output;
		int success=verifyHMACFunc(file,(char*)output);
		if(success==1)
			cout<<"HMAC verified succesfully...\n";
		else
			cout<<"HMAC was not verified correctly...someone has tampered the input file...\n";
	}
	else
		cout<<"You don't have permissions to read this file...\n";
}

