#include<iostream>
#include<stdlib.h>
#include <dirent.h>
#include<string.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<pwd.h>
#include<libgen.h>
#include<limits.h>
#include<fstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include<openssl/sha.h>
#include <openssl/err.h> 
#include <openssl/hmac.h>

#define BUFFER_LENGTH 256

typedef enum {READ,WRITE,EXECUTE} Rights;
typedef struct ACL_data{
	unsigned char **aclUsers;
	unsigned int aclLen;
}ACL;


void ERR_load_crypto_strings(void);

void ERR_free_strings(void);

char* getWorkingDirectory();
const char *getUserName();

int checkUserId(int userid);
uid_t getRealUserID();
uid_t getEffectiveUserID();
void restoreUserId(uid_t);
void changeUserId(uid_t);
char **shellSplitLine(char *line,int task);
int compareString(char *first, const char *second);
int checkReadPermission(char *permissionLine,const char *renter,ACL *acl);
int checkWritePermission(char *permissionLine,const char *renter,ACL *acl);
int checkExecutePermission(char *permissionLine,const char *renter,ACL *acl);
char readFileOfGroup(char *owner,const char *renter);
int checkDirectoryAccess(char *directoryName,const char *username,Rights right);
char* getAbsolutePath(char *workingDirectory);
void splitFileName(char **fname,char **dname,char **fileName);
int checkDirectoryExist(char *directoryPath);
int checkFileExist(char *filePath);
void getMessageFromConsole(char **message);
ACL getPermissionsFromFile(char *fileName,char **permissions);
int checkPermissionForChmod(char *permissionLine,const char *renter);
char* getDirectoryOwnerName(char *dirName);
void initializeAcls(FILE *fp,char *permissionsReceived);
void readACLFromFile(char *fileName);
ACL updateACLFromFile(char *fileName,char *potentialUser,char *changedPermission);
void readData(char *fileName,char **bufferSent);
void updateFileACL(char *fileName,ACL *acl,char *fileData,char *filePermissions);
void copyString(unsigned char *s1,unsigned char *s2);
int checkACLPermission(ACL *acl,const char *renter,Rights right);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
int aes_init(int operation,unsigned char *buffer,unsigned char **output,char *fileName);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *plaintext);
void addDataToFile(char *data,FILE *fp);
void renameHMACFile(char *fileName);
void signAMessage(char *message,char *passphrase,char *file);
size_t hmacFunction(char *message,char *passphrase,char **md_value);
int verifyHMACFunc(char *file,char *message);
void handleErrors();
