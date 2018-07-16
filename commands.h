#include <sys/types.h>
#include <sys/stat.h>

/* LOCAL */

int ls();
int cd(char filename[]);
int pwd();
int rm(char filename[]);

/* REMOTE */

int downl(int sock, char filename[], char buf[], size_t size, int index);

int upld(int sock, char filename[], char buf[], struct stat obj, size_t size);

char* rpwd(int sock, char buf[]);

int rls(int sock, char buf[], size_t size);

int rcd(int sock, char filename[], char buf[]);

int quit(int sock, char buf[]);

int rshutdown(int sock, char buf[]);
