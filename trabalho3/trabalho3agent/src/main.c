#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#define BSIZE 80
#define NOMEFIFO "/tmp/suporte"
#define PIPEADMIN "/tmp/admin"
#define MAX_DISCIPLINAS 10
#define MAX_HORARIOS 3

typedef struct
{
    int lugares;           
    int vagas;             
    pthread_mutex_t mutex; 
} Horario;

typedef struct
{
    char nome[BSIZE];
    Horario horarios[MAX_HORARIOS];
} Disciplina;

typedef struct
{
    int aluno_id;                   
    int num_alunos;                 
    char nome_pipe_resposta[BSIZE]; 
    char nome_pipe_resposta_admin[BSIZE];
    int codop;                      
    char nome_ficheiro[BSIZE];      
    int disciplina; 
} Pedido;

Disciplina disciplinas[MAX_DISCIPLINAS];

void inicializa_disciplinas()
{
    for (int i = 0; i < MAX_DISCIPLINAS; i++)
    {
        
        sprintf(disciplinas[i].nome, "Disciplina %d", i + 1);

        for (int j = 0; j < MAX_HORARIOS; j++)
        {
            disciplinas[i].horarios[j].lugares = 10;
            disciplinas[i].horarios[j].vagas = 10;
            pthread_mutex_init(&disciplinas[i].horarios[j].mutex, NULL);
        }
    }
}

int gravar_ficheiro(Pedido *msg)
{
    int fd_ficheiro;
    char nome_ficheiro[BSIZE];

    // Obter o nome do ficheiro do pedido
    snprintf(nome_ficheiro, sizeof(nome_ficheiro), "%s", msg->nome_ficheiro);

    fd_ficheiro = open(nome_ficheiro, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_ficheiro < 0)
    {
        perror("Erro ao abrir ficheiro");
        pthread_exit(NULL);
    }

    int num_alunos = 0;

    for (int aluno_id = 0; aluno_id < msg->num_alunos; aluno_id++)
    {
        char linha[BSIZE];
        int offset = 0;

        // Número do aluno
        offset += snprintf(linha + offset, sizeof(linha) - offset, "%d", aluno_id + 1);

        for (int i = 0; i < MAX_DISCIPLINAS; i++)
        {
            for (int j = 0; j < MAX_HORARIOS; j++)
            {
                pthread_mutex_lock(&disciplinas[i].horarios[j].mutex);
                if (disciplinas[i].horarios[j].vagas < 10)
                {
                    
                    offset += snprintf(linha + offset, sizeof(linha) - offset, ",%d", j);
                }
                else
                {
                    
                    offset += snprintf(linha + offset, sizeof(linha) - offset, ",");
                }
                pthread_mutex_unlock(&disciplinas[i].horarios[j].mutex);
            }
        }

        snprintf(linha + offset, sizeof(linha) - offset, "\n");
        write(fd_ficheiro, linha, strlen(linha));
    }

    close(fd_ficheiro);

    return num_alunos;
}

void *processa_pedido(void *arg)
{
    Pedido *msg = (Pedido *)arg;
    int horario_inscrito = -1;

    for (int j = 0; j < MAX_HORARIOS; j++) {
        pthread_mutex_lock(&disciplinas[msg->disciplina].horarios[j].mutex);

        
        if (disciplinas[msg->disciplina].horarios[j].vagas > 0) {
            
            disciplinas[msg->disciplina].horarios[j].vagas--;
            horario_inscrito = j;  
            pthread_mutex_unlock(&disciplinas[msg->disciplina].horarios[j].mutex);
            break;
        }
        pthread_mutex_unlock(&disciplinas[msg->disciplina].horarios[j].mutex);
    }

    int fd_resposta;
    if ((fd_resposta = open(msg->nome_pipe_resposta, O_WRONLY)) < 0)
    {
        perror("open (resposta)");
        pthread_exit(NULL);
    }

    char resposta[BSIZE];

    sprintf(resposta, "%d", horario_inscrito);

    write(fd_resposta, resposta, strlen(resposta) + 1);
    close(fd_resposta);

    pthread_exit(NULL);
}

void *processa_pedido_admin(void *arg)
{
    Pedido *msg = (Pedido *)arg; 
    char resposta[BSIZE];
    

    
    int fd_resposta;
    if ((fd_resposta = open(msg->nome_pipe_resposta_admin, O_RDONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

    
    switch (msg->codop)
    {
    case 1:
        printf("Pedir horários do aluno %d\n", msg->aluno_id);

        char resposta_horarios[BSIZE];
        snprintf(resposta_horarios, sizeof(resposta_horarios), "%d", msg->aluno_id);

        int primeiro_horario = 1;  
        for (int i = 0; i < MAX_DISCIPLINAS; i++)
        {
            for (int j = 0; j < MAX_HORARIOS; j++)
            {
                if (!primeiro_horario)
                {
                    snprintf(resposta_horarios + strlen(resposta_horarios), sizeof(resposta_horarios) - strlen(resposta_horarios), ",");
                }
                
                snprintf(resposta_horarios + strlen(resposta_horarios), sizeof(resposta_horarios) - strlen(resposta_horarios), "%s/%d", disciplinas[i].nome, j + 1);
                primeiro_horario = 0;
            }
        }
        
        write(fd_resposta, resposta_horarios, strlen(resposta_horarios) + 1);
        break;

    case 2:
        printf("Gravar no ficheiro %s\n", msg->nome_ficheiro);
        
        
        int num_alunos = gravar_ficheiro(msg); 

       
        if (num_alunos == -1)
        {
            snprintf(resposta, sizeof(resposta), "-1");
        }
        else
        {
            snprintf(resposta, sizeof(resposta), "%d", num_alunos);
        }
        
       
        write(fd_resposta, resposta, strlen(resposta) + 1);
        break;

    case 3:
        snprintf(resposta, sizeof(resposta), "Ok");
        write(fd_resposta, resposta, strlen(resposta) + 1);
        close(fd_resposta);
        pthread_exit(NULL);
        break;

    default:
        break;
    }

    close(fd_resposta);
    pthread_exit(NULL);
}

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
    int num_alunos;
    char buf[BSIZE];
    Pedido msg_recebida;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <número de alunos>\n", argv[0]);
        exit(1);
    }

    num_alunos = atoi(argv[1]);
    int fd;

    if ((fd = open(NOMEFIFO, O_RDONLY)) < 0)
    {
        perror("open");
        exit(1);
    }

    int fd_admin;

    if ((fd_admin = open(PIPEADMIN, O_RDONLY)) < 0)
    {
        perror("open (admin)");
        exit(1);
    }

    inicializa_disciplinas();
    // thread alunos
    pthread_t threads[num_alunos];
    Pedido pedidos[num_alunos];

    for (int i = 0; i < num_alunos; i++)
    {
        le_pipe(fd, buf, sizeof(buf));

        if (sscanf(buf, "%d %d %s",  &pedidos[i].aluno_id, &pedidos[i].disciplina, pedidos[i].nome_pipe_resposta) != 3)
        {
            fprintf(stderr, "Erro no pedido\n");
            close(fd);
            exit(1);
        }

        pthread_create(&threads[i], NULL, processa_pedido, &pedidos[i]);
    }

    while (1)
    {

        le_pipe(fd_admin, buf, sizeof(buf));

       
        if (sscanf(buf, "%d", &msg_recebida.codop) != 1) 
        {
            fprintf(stderr, "Erro ao ler codop do admin\n");
            close(fd_admin);
            exit(1);
        }

        
        if (msg_recebida.codop == 1) 
        {
            if (sscanf(buf, "%d,%d,%s", &msg_recebida.codop, &msg_recebida.aluno_id, &msg_recebida.nome_pipe_resposta_admin) != 3)
            {
                fprintf(stderr, "Erro na leitura da operação de alocação de lugares\n");
                continue; 
            }
        }
        else if (msg_recebida.codop == 2) 
        {
            if (sscanf(buf, "%d,%s,%s", &msg_recebida.codop, &msg_recebida.nome_ficheiro, &msg_recebida.nome_pipe_resposta_admin) != 3)
            {
                fprintf(stderr, "Erro na leitura da operação de gravação no ficheiro\n");
                continue;
            }
        }
        else if (msg_recebida.codop == 3) 
        {
            if (sscanf(buf, "%d,%s", &msg_recebida.codop, &msg_recebida.nome_pipe_resposta_admin) != 2)
            {
                fprintf(stderr, "Erro na leitura da operação de terminar\n");
                continue;
            }
        }

        pthread_t thread_admin;
        pthread_create(&thread_admin, NULL, processa_pedido_admin, NULL);

        pthread_join(thread_admin, NULL);
    }

    for (int i = 0; i < num_alunos; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(fd);
    close(fd_admin);

    return 0;
}