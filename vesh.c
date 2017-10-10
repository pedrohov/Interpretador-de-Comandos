#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
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

        // Array de palavras a esquerda do pipe:
        char *leftCommands[50];

        // Array de palavras a direita do pipe:
        char *rightCommands[50];

        // Trata a linha de comando recebida:
        int pipeFound = parse(cmd, leftCommands, rightCommands);

        // Executa o comando:
        execute(leftCommands, rightCommands, pipeFound);
        
    }
    
    return;
}

int parse(char *cmd, char *left[], char *right[]) {

    // Divide a linha em uma array de strings:
    char *commands[50];
    int indexCmd = 0;
    commands[indexCmd] = strtok(cmd, " ");
    indexCmd++;

    while(commands[indexCmd - 1] != NULL) {
        commands[indexCmd] = strtok(NULL, " ");
        indexCmd++;
    }

    // Remove o caractere de nova linha
    // do nome do comando, se existir:
    strtok(commands[indexCmd - 2], "\n");

    // Divide os comandos em esquerda e direita do pipe:
    left[0] = commands[0];
    int lIndex = 1;
    int rIndex = 0;

    // Procure por um pipe:
    int j;
    int pipeFound = 0;
    for(j = 1; j < indexCmd - 1; j++) {

        // Se encontrar o pipe:
        if(strcmp(commands[j], "|") == 0)
            pipeFound = 1;
        else if(strcmp(commands[j], ">") == 0)
            pipeFound = 2;
        else if(strcmp(commands[j], "<") == 0)
            pipeFound = 3;

        if(pipeFound != 0) {
            left[lIndex] = '\0';
            lIndex = lIndex + 1;

            // Cria um novo vetor de strings
            // com os comandos a direita do pipe:
            while(j + rIndex < indexCmd - 1) {
                right[rIndex] = commands[j + 1 + rIndex];
                rIndex = rIndex + 1;
            }
            break;
        }

        left[lIndex] = commands[j];
        lIndex = lIndex + 1;
    }

    left[lIndex] = NULL;
    lIndex++;

    return pipeFound;
}

void execute(char *left[], char *right[], int tPipe) {

    // Trata pipe do tipo '|':
    if(tPipe == 1) {
        int des_p[2];
        if(pipe(des_p) == -1) {
          perror("Falha na criacao do pipe.");
          exit(1);
        }

        if(fork() == 0)
        {
            // Fecha saida padrao:
            close(STDOUT_FILENO);

            // Redireciona saida para o Write end do pipe:
            dup(des_p[WRITE]);

            // Libera os descritores do pipe:
            close(des_p[WRITE]);
            close(des_p[READ]);

            // Executa o comando:
            execvp(left[0], left);
            perror("execvp falhou");
            exit(1);
        }

        if(fork() == 0)
        {
            // Fecha entrada padrao:
            close(STDIN_FILENO);

            // Redireciona entrada para o Read end do pipe:
            dup(des_p[READ]);

            // Libera os descritores do pipe:
            close(des_p[READ]);
            close(des_p[WRITE]);

            // Executa o comando:
            execvp(right[0], right);
            perror("execvp falhou");
            exit(1);
        }

        // Libera os descritores do pipe:
        close(des_p[WRITE]);
        close(des_p[READ]);

        wait(NULL);
        wait(NULL);
    }
    // Pipe do tipo '>':
    else if(tPipe == 2) {

        // Abre arquivo para escrita:
        int file = open(right[0], O_WRONLY);

        if(fork() == 0)
        {
            // Fecha saida padrao:
            close(STDOUT_FILENO);

            // Redireciona saida para o arquivo:
            dup(file);

            // Executa o comando a esquerda do pipe:
            execvp(left[0], left);
            perror("execvp falhou");
            exit(1);

        } else {
            // Fecha o arquivo:
            close(file);
            wait(NULL);
        }
    }
    // Pipe do tipo '<':
    else if (tPipe == 3) {

        // Abre arquivo para leitura:
        int file = open(right[0], O_RDONLY);

        if(fork() == 0)
        {
            // Fecha entrada padrao:
            close(STDIN_FILENO);

            // Redireciona entrada para dados do arquivo:
            dup(file);

            // Executa o comando a esquerda do pipe:
            execvp(left[0], left);
            perror("execvp falhou");
            exit(1);

        } else {
            // Fecha o arquivo:
            close(file);
            wait(NULL);
        }

    }
    // Se nao, cria apenas um:
    else {
        pid_t pid = fork();

        if(pid < 0) {
            printf("Fork falhou.\n");
        }
        else if(pid == 0) {
            execvp(left[0], left);
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