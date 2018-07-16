#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"

//loop client
int loop(int sock) {

    struct stat obj;
    size_t size;
    int success;
    char buf[128], filename[96], command[8], *response;

    int i = 1;
    //boucle qui s'occuppe de gerer ce que l'utilisateur rentre et 
    //executer la fonction correspondante 
    while(1)
    {
        printf("%s\n","Enter a command (ls, cd, pwd, rm, downl, upld, rpwd, rls, rcd, quit): ");
        scanf("%s", command);

        if(!strcmp(command, "ls")) {
            if(ls()) {
                printf("%s\n","The remote directory listing is as follows: ");
                system("cat ls.txt");
                system("rm ls.txt");
            }
        } else if(!strcmp(command, "cd")) {
            printf("%s\n","Enter the path to change the directory: ");
            scanf("%s", filename);
            printf("%s\n", cd(filename) ? "Directory changed successfully !" : "Error when trying to change directory");
        }  else if(!strcmp(command, "pwd")) {
            if(pwd()) {
                printf("%s\n","The current directory name is: \n");
                system("cat pwd.txt");
                system("rm pwd.txt");
            }
        }  else if(!strcmp(command, "rm")) {
            printf("%s\n","Enter the filename to remove: ");
            scanf("%s", filename);
            printf("%s\n", rm(filename) ? "File removed successfully !" : "Error when trying to remove file");
        } else if(!strcmp(command, "downl")) {
            printf("%s\n","Enter filename to get: ");
            scanf("%s", filename);
            success = downl(sock, filename, buf, size, i);
            printf(success ? "File successfully downloaded\n" : "Failed to download the requested file\n");
        } else if(!strcmp(command, "upld")) {
            printf("%s\n","Enter filename to put to server: ");
            scanf("%s", filename);
            success = upld(sock, filename, buf, obj, size);
            printf("%s\n", success ? "File successfully uploaded\n" : "Failed to upload the given file\n");
        } else if(!strcmp(command, "rpwd")) {
            response = rpwd(sock, buf);
            printf("%s%s\n","The current remote directory name is: ", response);
        } else if(!strcmp(command, "rls")) {
            success = rls(sock, buf, size);
            if(success) {
                printf("%s\n","The remote directory listing is as follows:\n");
                system("cat temp.txt");
                system("rm temp.txt");
            }
        } else if(!strcmp(command, "rcd")) {
            printf("%s\n", "Enter the path to change the remote directory: ");
            scanf("%s", filename);
            success = rcd(sock, filename, buf);
            printf("%s\n", success ? "Directory changed successfully !" : "Error when trying to change directory");
        //c'est cette commande qui sors de la boucle apres avoir fermé la connection
        } else if(!strcmp(command, "quit")) {
            success = quit(sock, buf);
            if(success) {
                printf("%s\n", "Server closed\nQuitting..\n");
                return 0;
            }
            printf("%s\n", "Server failed to close the connection\n");
        } else if(!strcmp(command, "shutdown")) {
            success = shutdown(sock, buf);
            if(success) {
                printf("%s\n", "Server closed\nQuitting..\n");
                return 0;
            }
            printf("%s\n", "Server failed to close the connection\n");
        } else {
            printf("%s\n", "Please choose a choice from the list.");
        }
    }
}

int main(int argc,char *argv[])
{
    struct sockaddr_in server;
    char buf[128];
    int sock, k;

    //on creer un socket en TCP, IPV4
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("socket creation failed");
        return 1;
    }

    //configuration adresse serveur
    server.sin_family = AF_INET;
    server.sin_port = htons(1977);
    server.sin_addr.s_addr = 0;

    //connection au serveur 
    k = connect(sock, (struct sockaddr*)&server, sizeof(server));
    if(k == -1) {
        printf("Connection error");
        return 1;
    }

    //envoie "ping" de présence
    send(sock, "BONJ", 128, 0);
    recv(sock, &buf, 128, 0);
    printf("%s\n", buf);


    if(strcmp(buf, "WHO") == 0) {
        //login
        scanf("%s", buf);
        send(sock, buf, 128, 0);
        recv(sock, &buf, 128, 0);
        printf("%s\n", buf);

        if(strcmp(buf, "PASSWD") == 0) {
            while (1) {
                //mot de passe 
                scanf("%s", buf);
                send(sock, buf, 128, 0);
                recv(sock, &buf, 128, 0);
                printf("%s\n", buf);

                // on lance la boucle de command ou on quitte 
                //si le password est incorrect
                if(strcmp(buf, "WELC") == 0) {
                    return loop(sock);
                } else if(strcmp(buf, "BYE") == 0) {
                    quit(sock, buf);
                    return 0;
                }
            }
        }
    }
}


