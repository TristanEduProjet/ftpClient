#include "commands.h"

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>



int ls() {
    system("ls>ls.txt");
    return 1;
}

int cd(char filename[]) {
    return chdir(filename) == 0 ? 1 : 0;
}


int pwd() {
    system("pwd>pwd.txt");
    return 1;
}

int rm(char filename[]) {
    system(strcat("rm", filename));
    return 1;
}



int downl(int sock, char filename[], char buf[], size_t size, int index)
{
    int fd;

    strcpy(buf, "downl ");
    strcat(buf, filename);

    send(sock, buf, 128, 0);
    recv(sock, &size, sizeof(int), 0);

    char* f = malloc(size);
    recv(sock, f, size, 0);

    fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
    write(fd, f, size);
    close(fd);

    return 1;
}

int upld(int sock, char filename[], char buf[], struct stat obj, size_t size)
{
    int fd = open(filename, O_RDONLY);
    int status;

    if(fd == -1)
    {
        printf("%s\n","No such file on the local directory.");
        return 0;
    }

    strcpy(buf, "upld ");
    strcat(buf, filename);
    send(sock, buf, 128, 0);
    stat(filename, &obj);
    size = (size_t) obj.st_size;
    send(sock, &size, sizeof(int), 0);

    char* content = malloc(size);
    int i = 0;
    FILE*f = fopen(filename,"r");
    while(!feof(f))
        content[i++] = (char) fgetc(f);
    content[i-1] = '\0';
    fclose(f);

    send(sock, content, size, 0);
    recv(sock, &status, sizeof(int), 0);

    return status;
}

char* rpwd(int sock, char buf[])
{
    strcpy(buf, "rpwd");

    send(sock, buf, 128, 0);
    recv(sock, buf, 128, 0);

    return buf;
}

int rls(int sock, char buf[], size_t size)
{
    strcpy(buf, "rls");
    send(sock, buf, 100, 0);
    recv(sock, &size, sizeof(unsigned long), 0);
    char* f = malloc(size);
    recv(sock, f, size, 0);
    int fd = open("temp.txt", O_CREAT | O_EXCL | O_WRONLY, 0666);
    write(fd, f, size);
    close(fd);
    return 1;
}

int rcd(int sock, char filename[], char buf[])
{
    int status;

    strcpy(buf, "rcd ");
    strcat(buf, filename);

    send(sock, buf, 128, 0);
    recv(sock, &status, sizeof(int), 0);

    return status;
}

int quit(int sock, char buf[])
{
    int status;

    strcpy(buf, "quit");
    send(sock, buf, 100, 0);
    recv(sock, &status, 100, 0);

    return status;
}

