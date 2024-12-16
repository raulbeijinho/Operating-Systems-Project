#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BSIZE 80
#define NOMEFIFO "/tmp/suporte"

int le_pipe(int fd, char *msg_in, int bsize)
{
    int j;
    for (j = 0; j < bsize; j++)
        msg_in[j] = 0;
    for (j = 0; j < bsize; j++)
    {
        if (read(fd, &msg_in[j], 1) < 0)
        {
            perror("le_pipe read");
            exit(1);
        }
        if (msg_in[j] == '\0')
            break;
    }
    return j;
}

int main(int argc, char const *argv[])
{
    int aluno_inicial;
    int num_alunos;
    int nstud;
    int dados = 0;
    char nome_pipe_resposta[BSIZE];
    char buf[BSIZE];
    char const *nome_pipe;
    
    if (argc != 4)
    {
        fprintf(stderr, "%s <nstud> <aluno_inicial> <num_alunos>\n", argv[0]);
        exit(1);
    }

    nstud = atoi(argv[1]);
    aluno_inicial = atoi(argv[2]);
    num_alunos = atoi(argv[3]);

    printf("student %d: aluno inicial=%d, número de alunos=%d\n", nstud, aluno_inicial, num_alunos);
    
    snprintf(nome_pipe_resposta, BSIZE, "/tmp/student_%d", nstud);

    if (mkfifo(nome_pipe_resposta, 0666) < 0)
    {
        perror("mkfifo");
        if (access(nome_pipe_resposta, F_OK) == 0)
        {
            printf("FIFO já existe\n");
        }
        else
        {
            printf("Erro ao criar FIFO.\n");
            exit(1);
        }
    } 
    
    int fd;
    nome_pipe = NOMEFIFO;
    
    if ((fd = open(nome_pipe, O_WRONLY)) < 0)
    {
        perror("open");
        exit(1);
    }
   
    
    for (int i = 0; i < num_alunos; i++)  
    {
        for (int j = 0; j < 5; j++) 
        {
            snprintf(buf, BSIZE, "%d,%d,%s\n", aluno_inicial + i, j, nome_pipe_resposta);  
            if (write(fd, buf, strlen(buf) + 1) < 0)
            {
                perror("write");
                exit(1);
            }
        }
    }
    
    close(fd);
    
    int fd_resposta;
    char resposta[BSIZE];
    
    if ((fd_resposta = open (nome_pipe_resposta, O_RDONLY)) < 0) {
        perror ("open");
        exit(1);
    }

    printf("pipe aberto\n");

    int horarios[5];  

    for (int i = 0; i < num_alunos; i++)  
    {
        
        memset(horarios, -1, sizeof(horarios));  

        for (int j = 0; j < 5; j++)  
        {
            le_pipe(fd_resposta, resposta, sizeof(resposta));  
            int horario = atoi(resposta);  
            horarios[j] = horario;  

        }

        
        printf("student %d, aluno %d: dis1/%d, dis2/%d, dis3/%d, dis4/%d, dis5/%d\n",
               nstud, aluno_inicial + i, horarios[0], horarios[1], horarios[2], horarios[3], horarios[4]);
    }
    
    close(fd_resposta);

    printf("student %d: alunos inscritos=%d\n", nstud, dados);

    return 0;
}