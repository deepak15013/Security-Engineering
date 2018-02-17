#include "functions.h"
using namespace std;

char pathNameBuffer[256];
char actualPath[PATH_MAX+1];
char hmacFileName[50];

const char *getUserName()
{
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
	if (pw)
	{
		return pw->pw_name;
	}
	else
		return NULL;
}

uid_t getRealUserID()
{
	uid_t ruserid=getuid();
	cout<<"Real User ID : "<<ruserid<<"\n";
	return ruserid;
}

uid_t getEffectiveUserID()
{
	uid_t euserid=geteuid();
	cout<<"Effective User ID : "<<euserid<<"\n";
	return euserid;
}

int checkUserId(int userid)
{
	int fakerootId=0;
	if(userid==fakerootId)
	{
		cout<<"You have permissions to access this directory...\n";
		return 1;
	}
	else
	{
		cout<<"You don't have permissions to access this directory...\n";
		return 0;
	}
}

void restoreUserId(uid_t euid)
{
	int status=0;
	status=seteuid(euid);
	if(status<0){
		cout<<"Could not set the user id...\n";
		exit(-1);
	}
}

void changeUserId(uid_t ruid)
{
	int status=0;
	status=seteuid(ruid);
	if(status<0){
		cout<<"Could not set the user id...\n";
		exit(-1);
	}
}

char* getWorkingDirectory()
{
	if(!getcwd(pathNameBuffer,sizeof(pathNameBuffer)))
	{
		cout<<"Error in getting current path\n";
		exit(-1);
	}
	else
	{
		cout<<"Current Working directory : "<<pathNameBuffer<<"\n";
		cout<<"UserName : "<<getUserName()<<"\n";
	}
	return pathNameBuffer;
}


int checkDirectoryAccess(char *directoryName,const char *username,Rights right)
{
	FILE *fp;
	const char *fileName="aclDir.acl";
	ACL *acl;

	acl=(ACL*)malloc(sizeof(ACL));	
	char permissions[64];
	char ch;
	int isAllowedToAccess=0;
	char buffer[256];
	int i=0,j=0;
	while(directoryName[i]!='\0')
	{
		buffer[j]=directoryName[i];
		j++;i++;
	}
	if(buffer[j-1]!='/'){
		buffer[j]='/';
		j++;
	}
	i=0;
	while(fileName[i]!='\0')
	{
		buffer[j]=fileName[i];
		j++;i++;
	}
	buffer[j]='\0';
	cout<<"\nBuffer : "<<buffer<<"\n\n";
	if(access(buffer, F_OK ) != -1 )
	{
		fp=fopen(buffer,"r");
		if(fp==NULL){
			cout<<"You don't have permissions to access this directory...\n";
			return 0;
		}		
		else
		{
			i=0;
			while((ch=getc(fp))!='\n')
			{
				permissions[i]=ch;
				i++;
			}
			permissions[i]='\0';
			fscanf(fp,"%u",&acl->aclLen);
			int i=0;
			acl->aclUsers=(unsigned char**)malloc(sizeof(unsigned char*)*acl->aclLen);
			cout<<"Inside update func\n";
			while(i<acl->aclLen)
			{
				acl->aclUsers[i]=(unsigned char*)malloc(sizeof(unsigned char)*64);			
				fscanf(fp,"%s",(acl->aclUsers[i]));
				i++;
			}		
			for(int i=0;i<acl->aclLen;i++)
			{
				cout<<"User : "<<acl->aclUsers[i]<<"\n";
			}
			fclose(fp);
			cout<<"File closed\n";
			if(right==READ)
				isAllowedToAccess=checkReadPermission(permissions,username,acl);
			else
			if(right==WRITE)
				isAllowedToAccess=checkWritePermission(permissions,username,acl);
			else
				isAllowedToAccess=checkExecutePermission(permissions,username,acl);
			return isAllowedToAccess;
		}
	}
	else
	{
	    	return 0;
	}
}


int checkIfFileDir(char *path)
{
	struct stat s;
	if( stat(path,&s) == 0 )
	{
		if( s.st_mode & S_IFDIR )
		{
			return 1;			
			//it's a directory
		}
		else if( s.st_mode & S_IFREG )
		{
			return 2;
			//it's a file
		}
		else
		{
			cout<<"\nEnter a file name or a directory name...\n";
			exit(-1);
		}
	}
	else
	{
		cout<<"\nEnter a valid path...\n";
		exit(-1);
	}
	return 0;
}


//readWriteFunctions
//Split line
char **shellSplitLine(char *line,int task)
{
	char bufferSize=64;
	int position=0;
	char ** tokenArray=(char**)malloc(bufferSize*sizeof(char*));
	char *individualToken=NULL;
	if(tokenArray==NULL)
	{
		printf("Error in allocation\n");
		exit(-1);
	}
	if(task==1)
		individualToken=strtok(line," \t\r\n\a");
	else
	if(task==0)
		individualToken=strtok(line,":");
	else
	if(task==2)
		individualToken=strtok(line,"/");
	else
		individualToken=strtok(line,",");

	while(individualToken!=NULL)
	{
		tokenArray[position]=individualToken;
		position++;
		if(position>=bufferSize)
		{
			bufferSize+=64;
			tokenArray=(char**)realloc(tokenArray,bufferSize*sizeof(char*));
			if(tokenArray==NULL)
			{
				printf("Error in reallocation\n");
				exit(-1);
			}
		}
		if(task==1)
			individualToken=strtok(NULL," \t\r\n\a");
		else
		if(task==0)
			individualToken=strtok(NULL,":");
		else
		if(task==2)
			individualToken=strtok(NULL,"/");
		else
			individualToken=strtok(NULL,",");
	}
	free(individualToken);
	tokenArray[position]=NULL;
	return tokenArray;
}


int compareString(char *first,const char *second) {
	while (*first == *second) {
	        if (*first == '\0' || *second == '\0'){
		        break;
		}
		first++;
		second++;
   	}
	if (*first == '\0' && *second == '\0'){
		return 1;
	}
   	else{
		return 0;
	}
}

char readFileOfGroup(char *owner,const char *renter)
{
	FILE *filePointer;
	char *group,ch;
	char *lineInFile=NULL;
	size_t length=0;
	ssize_t read;
	char ** tokenArray;
	char ** userArray, **groupArrayAfterSplit;
	int temp1=0,temp2=0,i;
	int flag=0;
	char groupArray[256];

	filePointer=fopen("/simple_slash/home/groups.txt","r");
	if(filePointer==NULL){
		printf("Error in opening file\n");
	}
	printf(" %s %s \n",owner,renter);

	i=0;
	while((ch=getc(filePointer))!=EOF)
	{
		i=0;
		while(ch!='\n' && ch!=EOF)
		{	
			groupArray[i]=ch;
			i++;
			ch=getc(filePointer);
		}
		groupArray[i]='\0';
		groupArrayAfterSplit=shellSplitLine(groupArray,0);
		userArray=shellSplitLine(groupArrayAfterSplit[1],3);
		i=0;
		temp1=0,temp2=0;flag=0;
		while(userArray[i]!=NULL)
		{
			temp1=compareString(userArray[i],owner);
			temp2=compareString(userArray[i],renter);
			if(temp1==1)
				flag++;
			if(temp2==1)
				flag++;
			i++;
		}
		//group=tokenArray[0];
		if(flag==2)
		{
			break;
		}
	}
	fflush( stdout );
	fclose(filePointer);
	if(flag==2)
	{
		return 'y';
	}
	else
	{
		return 'n';
	}
}

int checkReadPermission(char *permissionLine,const char *renter,ACL *acl)
{
	int isOwnerRenterEqual=0;
	char *owner;
	char isGroupMember='\0';
	char **tokenArray;
	tokenArray=shellSplitLine(permissionLine,0);
	owner=tokenArray[0];
	Rights right=READ;
	isOwnerRenterEqual=compareString(owner,renter);
	printf("Owner : %s %s %d\n",owner,tokenArray[1],isOwnerRenterEqual);
	if(isOwnerRenterEqual==1)
	{
		if(tokenArray[1][1]=='r')
			return 1;
		else
			return 0;
	}
	else
	{
		isGroupMember=readFileOfGroup(owner,renter);
		printf("Group Member : %c\n",isGroupMember);
		if(isGroupMember=='y')
		{
			if(tokenArray[1][4]=='r')
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}
		}
		else
		{
			if(tokenArray[1][7]=='r')
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}
		}
	}
}

int checkACLPermission(ACL *acl,const char *renter,Rights right)
{
	int i=0,flag=0;
	char **userArray;
	while(i<acl->aclLen)
	{
		flag=0;
		userArray=shellSplitLine((char*)acl->aclUsers[i],0);
		if(compareString(userArray[0],renter)==1)
		{
			if(right==READ)
			{
				if(userArray[1][0]=='r')
					flag=1;
				else
					flag=0;
			}
			else
			if(right==WRITE)
			{
				if(userArray[1][1]=='w')
					flag=1;
				else
					flag=0;
			}
			else
			if(right==EXECUTE)
			{
				if(userArray[1][2]=='x')
					flag=1;
				else
					flag=0;
			}
			else
				flag=0;
			break;
		}
		i++;
	}
	if(flag==1)
		return 1;
	else
		return 0;
}

int checkPermissionForChmod(char *permissionLine,const char *renter)
{
	int isOwnerRenterEqual=0;
	char *owner;
	char isGroupMember='\0';
	char **tokenArray;
	tokenArray=shellSplitLine(permissionLine,0);
	owner=tokenArray[0];
	isOwnerRenterEqual=compareString(owner,renter);
	printf("Owner : %s %s %d\n",owner,tokenArray[1],isOwnerRenterEqual);
	if(isOwnerRenterEqual==1)
	{
		if(tokenArray[1][2]=='w')
			return 1;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

int checkWritePermission(char *permissionLine,const char *renter,ACL *acl)
{
	int isOwnerRenterEqual=0;
	char *owner;
	char isGroupMember='\0';
	char **tokenArray;
	tokenArray=shellSplitLine(permissionLine,0);
	owner=tokenArray[0];
	Rights right=WRITE;
	isOwnerRenterEqual=compareString(owner,renter);
	printf("Owner : %s %s %d\n",owner,tokenArray[1],isOwnerRenterEqual);
	if(isOwnerRenterEqual==1)
	{
		if(tokenArray[1][2]=='w')
			return 1;
		else
			return 0;
	}
	else
	{
		isGroupMember=readFileOfGroup(owner,renter);
		printf("Group Member : %c\n",isGroupMember);
		if(isGroupMember=='y')
		{
			if(tokenArray[1][5]=='w')
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}
		}
		else
		{
			if(tokenArray[1][8]=='w')		
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}		
		}
	}
}


int checkExecutePermission(char *permissionLine,const char *renter,ACL *acl)
{
	int isOwnerRenterEqual=0;
	char *owner;
	char isGroupMember='\0';
	char **tokenArray;
	tokenArray=shellSplitLine(permissionLine,0);
	owner=tokenArray[0];
	Rights right=EXECUTE;
	isOwnerRenterEqual=compareString(owner,renter);
	printf("Owner : %s %s %d\n",owner,tokenArray[1],isOwnerRenterEqual);
	if(isOwnerRenterEqual==1)
	{
		if(tokenArray[1][2]=='w')
			return 1;
		else
			return 0;
	}
	else
	{
		isGroupMember=readFileOfGroup(owner,renter);
		printf("Group Member : %c\n",isGroupMember);
		if(isGroupMember=='y')
		{
			if(tokenArray[1][6]=='x')
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}
		}
		else
		{
			if(tokenArray[1][9]=='x')		
				return 1;
			else
			{
				if(checkACLPermission(acl,renter,right))
					return 1;
				else
					return 0;
			}
		}
	}
}


char* getAbsolutePath(char *workingDirectory)
{
	char *ptr;
	ptr=realpath(workingDirectory,actualPath);
	cout<<"AbsolutePath : "<<actualPath<<"\n";
	return actualPath;
}

void splitFileName(char **fname,char **dname,char **fileName)
{
	char *dirc,*basec;
	dirc = strdup(*fileName);
	basec = strdup(*fileName);
	*dname = dirname(dirc);
	*fname = basename(basec);
	printf("dirname=%s, basename=%s\n", *dname, *fname);
}

void copyDirectoryName(char *name,char *rootDir)
{
	int i=0;
	while(name[i]!='\0')
	{
		rootDir[i]=name[i];
		i++;
	}
	int len=strlen(name);
	if(name[len-1]!='/')
		rootDir[len]='/';rootDir[len+1]='\0';
}



int checkDirectoryExist(char *directoryPath)
{
	char rootDir[PATH_MAX];
	cout<<"Path : "<<directoryPath<<"\n";
	copyDirectoryName(directoryPath,rootDir);
	cout<<"Path : "<<rootDir<<"\n";
	DIR* dir = opendir(rootDir);
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
		return 1;
	}
	else
		return 0;
}

int checkFileExist(char *filePath)
{
	if( access( filePath, F_OK ) != -1 )
		return 1;
	else 
		return 0;
}

void getMessageFromConsole(char **message)
{
	int position=0;
	char ch;
	int length=BUFFER_LENGTH;
	char *newBuffer=NULL;
	newBuffer=(char *)calloc(length,sizeof(char));
	if(newBuffer==NULL)
	{
		cout<<"Error in allocation...\n";
		exit(-1);
	}
	ch=fgetc(stdin);
	while(ch!=EOF)
	{
		newBuffer[position]=ch;
		position++;
		if(position>=length)
		{
			length+=64;
			newBuffer=(char*)realloc(newBuffer,length*sizeof(char));
			if(newBuffer==NULL)
			{
				cout<<"Error in reallocatiion...\n";
				exit(-1);
			}
		}
		ch=fgetc(stdin);
	}
	newBuffer[position]='\0';
	*message=newBuffer;
	cout<<"Buffer allocated\n";
}


ACL getPermissionsFromFile(char *fileName,char **permissions)
{
	FILE *fp;
	fp=fopen(fileName,"r");
	char ch;
	ACL *acl;
	acl=(ACL*)malloc(sizeof(ACL));
	char *tempArray=(char*)malloc(sizeof(char)*64);
	int i=0;
	if(fp!=NULL)
	{
		i=0;
		while((ch=getc(fp))!='\n')
		{
			tempArray[i]=ch;
			i++;
		}
		tempArray[i]='\0';
		*permissions=tempArray;
		fscanf(fp,"%u",&acl->aclLen);
		int i=0;
		acl->aclUsers=(unsigned char**)malloc(sizeof(unsigned char*)*acl->aclLen);
		cout<<"Inside update funcnknjhhjhjg\n";
		while(i<acl->aclLen)
		{
			acl->aclUsers[i]=(unsigned char*)malloc(sizeof(unsigned char)*64);			
			fscanf(fp,"%s",(acl->aclUsers[i]));
			i++;
		}		
		for(int i=0;i<acl->aclLen;i++)
		{
			cout<<"User : "<<acl->aclUsers[i]<<"\n";
		}
		fclose(fp);
	}
	fp=NULL;
	cout<<"vndknnb\n";
	return *acl;
}

char* getDirectoryOwnerName(char *dirName)
{
	struct stat info;
	stat(dirName, &info);  // Error check omitted
	struct passwd *pw = getpwuid(info.st_uid);
//	struct group  *gr = getgrgid(info.st_gid);
	if (pw != 0)
	{
		return pw->pw_name;
	}
	else
	{
		return NULL;
	}
}


void readData(char *fileName,char **bufferSent)
{
	FILE *fp=NULL;
	char ch;
	int length=BUFFER_LENGTH-1;
	char *newBuffer=(char*)malloc(sizeof(char)*BUFFER_LENGTH);
	int position=0;
	fp=fopen(fileName,"r");
	if(fp!=NULL)
	{
		while((ch=getc(fp))!=EOF)
		{
			newBuffer[position]=ch;
			position++;
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
		fclose(fp);
		*bufferSent=newBuffer;
		cout<<"Buffer : "<<newBuffer<<"\n";
	}
	else
	{
		cout<<"Error in opening file...\n";
		exit(-1);
	}
}


int aes_init(int operation,unsigned char *buffer,unsigned char **output,char *fileName)
{
  unsigned char key[32];
  unsigned char iv[16];

  unsigned char *ciphertext=(unsigned char*)malloc(128*sizeof(unsigned char));
  unsigned char *decryptedtext=(unsigned char*)malloc(128*sizeof(unsigned char));
  int nrounds = 5;
  int decryptedtext_len, ciphertext_len;
	cout<<"Input received : "<<buffer<<"\n";

	int passphrase_len, i;
	char passphrase[64];
	cout<<"\nEnter passphrase : ";
	cin>>passphrase;

	unsigned char digest[SHA512_DIGEST_LENGTH];
	char salt[SHA512_DIGEST_LENGTH*2+1];
	cout<<passphrase;

	SHA512((unsigned char*)&passphrase, strlen(passphrase), (unsigned char*)&digest);    

	for(int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		sprintf(&salt[i*2], "%02x", (unsigned int)digest[i]);

	printf("Salt : %s\n", salt);

//	passphrase = (unsigned char *)passphrase;
	passphrase_len = strlen((const char*)passphrase);

	i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(),(const unsigned char*)salt,(const unsigned char*)passphrase, passphrase_len, nrounds, (unsigned char*)key, (unsigned char*)iv);
	if (i != 32) {
		printf("Key size is %d bits - should be 256 bits\n", i);
		return -1;
	}

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);
  cout<<strlen ((char *)buffer)<<"\n";
  if(operation==1){
	  /* Encrypt the plaintext */
	  ciphertext_len = encrypt (buffer, strlen ((char *)buffer), key, iv,
		                    ciphertext);

	  /* Do something useful with the ciphertext here */
	  printf("Ciphertext is:\n");
	  signAMessage((char*)buffer,passphrase,fileName);
	  BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);
	  *output=ciphertext;
  }
  else{
	  /* Decrypt the ciphertext */
	  i=0;
	  while(buffer[i]!='\0'){
		ciphertext[i]=buffer[i];
		i++;
	  }
	  ciphertext[i]='\0';
	  cout<<"cipher text : "<<ciphertext<<"\n"<<strlen((char*)ciphertext)<<"\n";
	  decryptedtext_len = decrypt(ciphertext,strlen((char*)ciphertext), key, iv,
	    decryptedtext);

	  /* Add a NULL terminator. We are expecting printable text */
	  
	  decryptedtext[decryptedtext_len] = '\0';
	cout<<decryptedtext_len<<"\n";
	  /* Show the decrypted text */
	  printf("Decrypted text is:\n");
	  printf("%s\n", decryptedtext);
	  *output=decryptedtext;
	  cout<<*output<<"\n";
  }

  EVP_cleanup();
  ERR_free_strings();
  return 0;
}


int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;
  
	
  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;
  int len=0;
  int plaintext_len=0;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
	
  plaintext_len = len;
	
  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
	cout<<plaintext_len<<" "<<len<<" "<<ciphertext_len<<"\n";	
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
	cout<<plaintext_len<<" "<<len<<" "<<ciphertext_len<<"\n";
  //plaintext_len += len;
	plaintext_len += len;
	cout<<plaintext_len<<" "<<len<<" "<<ciphertext_len<<"\n";
  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}




void renameHMACFile(char *fileName)
{
	int i=0;
	while(fileName[i]!='\0'){
		hmacFileName[i]=fileName[i];
		i++;
	}
	int j=0;
	char extension[]=".hmac";
	while(extension[j]!='\0'){
		hmacFileName[i]=extension[j];
		i++;j++;
	}
	hmacFileName[i]='\0';
}


void addDataToFile(char *data,FILE *fp)
{
	int i=0;
	while(data[i]!='\0')
	{
		putc(data[i],fp);
		i++;
	}
}

void signAMessage(char *message,char *passphrase,char *file)
{
	char *md_value;
	int hmacLength=hmacFunction(message,passphrase,&md_value);
	//Writing HMAC to file
	renameHMACFile(file);
	char *fileName=hmacFileName;
	cout<<fileName<<"\n";
	FILE *fp;
	fp=fopen(fileName,"w");
	if(fp!=NULL){
		addDataToFile((char*)md_value,fp);
		cout<<"hello\n";
		fclose(fp);
		cout<<"\nHMAC entered succesfully...\n";
	}
	else
		cout<<"Error in writing HMAC...\n";
}

void handleErrors()
{
	cout<<"Error occurred...\n";
	ERR_print_errors_fp(stderr);
}

size_t hmacFunction(char *message,char *passphrase,char **mdhash)
{
	EVP_MD_CTX* mdctx = NULL;
	const EVP_MD* md = NULL;
	EVP_PKEY *pkey = NULL;  
	size_t md_len = 0;
	unsigned char hmac[EVP_MAX_MD_SIZE];

	OpenSSL_add_all_digests();

	if(!(mdctx = EVP_MD_CTX_create()))
	    handleErrors();

	if(!(md = EVP_get_digestbyname("SHA1")))
	    handleErrors();

	cout<<"\n"<<strlen(passphrase)<<"\n";
	if(!(pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL,(unsigned char*)passphrase, strlen(passphrase))))
	    handleErrors();

	if(1 != EVP_DigestSignInit(mdctx, NULL, md, NULL, pkey))
	    handleErrors();

	if(1 != EVP_DigestSignUpdate(mdctx, message, strlen(message)))
	    handleErrors();

	if(1 != EVP_DigestSignFinal(mdctx, hmac, &md_len))
	    handleErrors();

	printf("HMAC is: ");
	char *temp=(char*)malloc(sizeof(char)*EVP_MAX_MD_SIZE);
	
	for(int i = 0; i < md_len; i++)
	    sprintf(&temp[i*2],"%02x",(unsigned int)hmac[i]);

	*mdhash=temp;
	cout<<"temp : "<<*mdhash<<"\n"<<md_len<<"\n";
	printf("\n");
	return md_len;
}

int verifyHMACFunc(char *file,char *message)
{
	char *md_value;
	char passphrase[64];
	cout<<"\nEnter passphrase : ";
	cin>>passphrase;

	int hmacLength=hmacFunction(message,passphrase,&md_value);

	renameHMACFile(file);
	char *fileName=hmacFileName;
	char *bufferSent;

	readData(fileName,&bufferSent);

	int flag=1;
	cout<<"MD value : "<<md_value;
	cout<<"\nhmacVer : "<<bufferSent<<"\n";

	for(int i=0;i<SHA_DIGEST_LENGTH*2+1;i++){
		if(bufferSent[i]!=md_value[i]){
			flag=0;
			break;
		}
	}
	return flag;
}

