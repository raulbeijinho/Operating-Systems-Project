#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BSIZE 80
#define PIPE_ADMIN "/tmp/admin"

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

int pedir_horarios(char *resposta)
{
    int num_aluno_resposta;
    int disciplina, horario;

    // Primeiro, extrair o número do aluno (até a primeira ",")
    if (sscanf(resposta, "%d,", &num_aluno_resposta) != 1)
    {
        fprintf(stderr, "Erro ao ler o número do aluno\n");
        return 1;
    }

    // Exibir o número do aluno
    printf("Aluno %d está inscrito nas seguintes disciplinas e horários:\n", num_aluno_resposta);

    // Processar o restante da resposta (disciplinas e horários)
    char *ptr = strchr(resposta, ','); // Avançar para depois do número do aluno
    if (ptr != NULL)
    {
        ptr++; // Pular o ","
    }

    while (ptr != NULL)
    {
        // Usar sscanf para ler cada par disciplina/horário
        if (sscanf(ptr, "%d/%d", &disciplina, &horario) == 2)
        {
            printf("Disciplina: %d, Horário: %d\n", disciplina, horario);
        }

        // Avançar para o próximo par disciplina/horário
        ptr = strchr(ptr, ','); // Procurar pela próxima vírgula
        if (ptr != NULL)
        {
            ptr++; // Pular a vírgula
        }
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    int codop;
    int admin_id;
    int num_aluno;
    char nome_pipe_resposta[BSIZE];
    char buf[BSIZE];
    char const *nome_pipe;
    char nome_ficheiro[BSIZE];

    if (argc != 2)
    {
        fprintf(stderr, "%s <admin_id>\n", argv[0]);
        exit(1);
    }

    admin_id = atoi(argv[1]);

    printf("Para PEDIR_HORÁRIOS escreva 1\n Para GRAVAR_FICHEIRO escreva 2\n Para TERMINAR escreva 3\n");
    scanf("%d", &codop);

    snprintf(nome_pipe_resposta, BSIZE, "/tmp/admin_%d", admin_id);

   
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

    // pedidos suporte agent
    
        int fd;
        nome_pipe = PIPE_ADMIN;

        if ((fd = open(nome_pipe, O_WRONLY)) < 0)
        {
            perror("open");
            exit(1);
        }

        // pedido codop 1
        if(codop == 1) {

        snprintf(buf, BSIZE, "%d,%d,%s\n", codop, num_aluno, nome_pipe_resposta);
        if (write(fd, buf, strlen(buf) + 1) < 0)
        {
            perror("write");
            exit(1);
        }
        }

        // pedido codop 2
        if(codop == 2) {
        snprintf(buf, BSIZE, "%d,%s\n", codop, nome_ficheiro);
        if (write(fd, buf, strlen(buf) + 1) < 0)
        {
            perror("write");
            exit(1);
        }
        }

        // pedido codop 3
        if(codop == 3) {
        snprintf(buf, BSIZE, "%d,%s\n", codop, nome_pipe_resposta);
        if (write(fd, buf, strlen(buf) + 1) < 0)
        {
            perror("write");
            exit(1);
        }
        }

        close(fd);
    
    

    int fd_resposta;
    char resposta[BSIZE];
    
    

    
    
    if ((fd_resposta = open(nome_pipe_resposta, O_RDONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

     

    if (codop == 1)
    {
        le_pipe(fd_resposta, resposta, sizeof(resposta));
        pedir_horarios(resposta);
        
    }

    if (codop == 2)
    {
        le_pipe(fd_resposta, resposta, sizeof(resposta));
        int num_alunos;

        
        num_alunos = atoi(resposta);

        if (num_alunos == -1)
        {
           
            printf("Erro na gravação do ficheiro.\n");
        }
        else
        {
            
            printf("Número de alunos gravados no ficheiro: %d\n", num_alunos);
        }
        
    }

    if (codop == 3)
{
    le_pipe(fd_resposta, resposta, sizeof(resposta));

    
    resposta[strcspn(resposta, "\n")] = 0;  
    
    
    if (strcmp(resposta, "Ok") == 0)
    {
        
        printf("Operação 'Terminar' concluída com sucesso.\n");
    }
    else
    {
        
        printf("Erro ao tentar terminar o processo.\n");
    }
}

    close(fd_resposta);
}