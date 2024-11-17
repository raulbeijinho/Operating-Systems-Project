#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BSIZE 80
#define NOMEFIFO "/tmp/suporte"

int fd;
pid_t pid;

int main(int argc, char const *argv[])
{
    int aluno_inicial;
    int num_alunos;
    char nome_pipe_resposta[BSIZE];
    char msg1[BSIZE];

    if (argc != 4)
    {
        fprintf(stderr, "%s <aluno_inicial> <num_alunos> <nome_pipe_resposta>\n", argv[0]);
        exit(1);
    }

    aluno_inicial = atoi(argv[1]);
    num_alunos = atoi(argv[2]);
    strcpy(nome_pipe_resposta, argv[3]);

    snprintf(msg1, BSIZE, "%d %d %s", aluno_inicial, num_alunos, nome_pipe_resposta);

    if ((fd = open(NOMEFIFO, O_WRONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

    if (write(fd, msg1, strlen(msg1) + 1) < 0)
    {
        perror("write");
        close(fd);
        exit(1);
    }

    write(fd, msg1, strlen(msg1) + 1);

    sleep(1);
    close(fd);
    return 0;
}