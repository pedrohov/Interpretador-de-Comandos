#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "vesh.h"

#define WRITE   1
#define READ    0

void vesh(void) {
    
    while(1) {
        // Indicar leitura de comandos:
        printf("$ ");

        // Le uma linha do terminal:
        char cmd[256];
        fgets(cmd, 256, stdin);

        // Comando para sair do vesh:
        if(strcmp(cmd, "sair\n") == 0) {
            break;
        }

        // Array de palavras:
        char *commands[50];
        int indexCmd = 0;
        int pipeFound = 0;
        char *leftCommands[50];
        int indexLcmd = 0;
        char *rightCommands[50];
        int indexRcmd = 0;

        // Divide a linha em uma array de strings:
        commands[indexCmd] = strtok(cmd, " ");
        indexCmd++;

        while(commands[indexCmd - 1] != NULL) {
            commands[indexCmd] = strtok(NULL, " ");
            indexCmd++;
        }

        // Remove o caractere de nova linha
        // No nome do comando, se existir:
        strtok(commands[indexCmd - 2], "\n");

        // Divide os comandos em esquerda e direita do pipe:
        leftCommands[0] = commands[0];
        indexLcmd++;

        // Procure por um pipe:
        int j;
        for(j = 1; j < indexCmd - 1; j++) {

            // Se encontrar o pipe:
            if(strcmp(commands[j], "|") == 0) {

                leftCommands[indexLcmd] = '\0';
                indexLcmd++;

                // 
                pipeFound = 1;

                // Cria um novo vetor de strings
                // com os comandos a direita do pipe:
                while(j + indexRcmd < indexCmd - 1) {
                    rightCommands[indexRcmd] = commands[j + 1 + indexRcmd];
                    indexRcmd++;
                }

                break;
            }

            leftCommands[indexLcmd] = commands[j];
            indexLcmd++;
        }

        leftCommands[indexLcmd] = NULL;
        indexLcmd++;

        // Mostra os vetores:
        /*printf("Left:\n");
        int k;
        for(k = 0; k < indexLcmd; k++)
            printf("%s ", leftCommands[k]);
        printf("\nRight:\n");
        for(k = 0; k < indexRcmd; k++)
            printf("%s ", rightCommands[k]);
        printf("\n");*/

        // Se houver pipe cria dois filhos:
        // Trata pipe do tipo '|':
        if(pipeFound == 1) {

            int des_p[2];
            if(pipe(des_p) == -1) {
              perror("Falha na criacao do pipe.");
              exit(1);
            }

            if(fork() == 0)
            {
                close(STDOUT_FILENO);
                dup(des_p[WRITE]);
                close(des_p[WRITE]);

                close(des_p[READ]);

                execvp(leftCommands[0], leftCommands);
                perror("execvp falhou");
                exit(1);
            }

            if(fork() == 0)
            {
                close(STDIN_FILENO);
                dup(des_p[READ]);
                close(des_p[READ]);

                close(des_p[WRITE]);

                execvp(rightCommands[0], rightCommands);
                perror("execvp falhou");
                exit(1);
            }

            close(des_p[WRITE]);
            close(des_p[READ]);

            wait(NULL);
            wait(NULL);
        }

        // Se nao, cria apenas um:
        else {
            pid_t pid = fork();

            if(pid < 0) {
                printf("Fork falhou.\n");
            }
            else if(pid == 0) {
                execvp(leftCommands[0], leftCommands);
                perror("execvp falhou");
                exit(1);
            }
            else {
                // Faz o processo pai esperar pelo
                // processo filho:
                wait(NULL);
            }
        }
    }
    
    return;
}

void parser(const char *cmd) {
    return;
}
