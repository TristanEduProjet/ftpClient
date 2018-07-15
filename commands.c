#include "commands.h"

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


//liste les fichiers 
int ls() {
    system("ls>ls.txt");
    return 1;
}

//change directory
int cd(char filename[]) {
    return chdir(filename) == 0 ? 1 : 0;
}

//recupere le working directory
int pwd() {
    system("pwd>pwd.txt");
    return 1;
}

//supprime le fichier
int rm(char filename[]) {
    system(strcat("rm", filename));
    return 1;
}


//dl un fichier 
int downl(int sock, char filename[], char buf[], size_t size, int index)
{
    int fd;

    //creer la commande à envoyer
    strcpy(buf, "downl ");
    strcat(buf, filename);

    //envoie la commande 
    send(sock, buf, 128, 0);
    //receive la taille à télécharger
    recv(sock, &size, sizeof(int), 0);

    //alloue la memoire puis dl le fichier 
    char* f = malloc(size);
    recv(sock, f, size, 0);

    //ecrit le fichier 
    fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
    write(fd, f, size);
    close(fd);

    return 1;
}

//upload un fichier sur le serveur 
int upld(int sock, char filename[], char buf[], struct stat obj, size_t size)
{
    //filedescriptor du fichier en readonly
    int fd = open(filename, O_RDONLY);
    int status;

    //si erreur de lecture on affiche l'erreur associé 
    if(fd == -1)
    {
        printf("%s\n","No such file on the local directory.");
        return 0;
    }

    //on prepare la commande 
    strcpy(buf, "upld ");
    strcat(buf, filename);
    //on envoie la commande 
    send(sock, buf, 128, 0);
    //on recupere les infos du fichier 
    stat(filename, &obj);
    //on envoie la taille du fichier
    size = (size_t) obj.st_size;
    send(sock, &size, sizeof(int), 0);

    //on charge le contenu du fichier en memoire 
    char* content = malloc(size);
    int i = 0;
    FILE*f = fopen(filename,"r");
    while(!feof(f))
        content[i++] = (char) fgetc(f);
    content[i-1] = '\0';
    fclose(f);

    //on envoie le fichier et on receive le statut retourné par le serveur 
    send(sock, content, size, 0);
    recv(sock, &status, sizeof(int), 0);

    return status;
}

//remote pathname working directory
char* rpwd(int sock, char buf[])
{
    strcpy(buf, "rpwd");

    send(sock, buf, 128, 0);
    recv(sock, buf, 128, 0);

    return buf;
}

//remote ls
int rls(int sock, char buf[], size_t size)
{
    //on prepare et envoie la commande 
    strcpy(buf, "rls");
    send(sock, buf, 100, 0);

    //le serveur renvoie la taille du listing 
    recv(sock, &size, sizeof(unsigned long), 0);

    //recois le listing depuis le serveur 
    char* f = malloc(size);
    recv(sock, f, size, 0);

    //stocke temporairement le listing dans un fichier
    //ouvert en ecriture, lock exclusive 
    int fd = open("temp.txt", O_CREAT | O_EXCL | O_WRONLY, 0666);
    write(fd, f, size);
    close(fd);
    return 1;
}

//remote change directory
int rcd(int sock, char filename[], char buf[])
{
    int status;

    strcpy(buf, "rcd ");
    strcat(buf, filename);

    send(sock, buf, 128, 0);
    recv(sock, &status, sizeof(int), 0);

    return status;
}

//ferme la session ftp
int quit(int sock, char buf[])
{
    int status;

    strcpy(buf, "quit");
    send(sock, buf, 100, 0);
    recv(sock, &status, 100, 0);

    return status;
}

