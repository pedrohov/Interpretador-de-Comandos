#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "vesh.h"

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
        char *commands[10];
        int indexCmd = 0;

        // Divide a linha em uma array de strings:
        commands[indexCmd] = strtok(cmd, " ");
        indexCmd++;

        while(commands[indexCmd - 1] != NULL) {
            commands[indexCmd] = strtok(NULL, " ");
            indexCmd++;
        }

        // Remove caractere de nova linha
        // No nome do comando, se existir:
        strtok(commands[indexCmd - 2], "\n");

        // Cria processo filho:
        pid_t pid = fork();

        if(pid < 0) {
            printf("Fork Failed.\n");
        }
        else if(pid == 0) {
            execvp(commands[0], commands);
            printf("Erro ao executar o comando %s.\n", commands[0]);
        }
        else {
            // Faz o processo pai esperar pelo
            // processo filho:
            wait(NULL);
        }
    }

    return;
}

void parser(const char *cmd) {
    return;
}
