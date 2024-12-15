#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#define BSIZE 80
#define NOMEFIFO "/tmp/suporte"
#define MAX_DISCIPLINAS 5
#define MAX_HORARIOS 3

typedef struct
{
    int lugares;           // Número total de lugares no horário
    int vagas;             // Número de vagas restantes
    pthread_mutex_t mutex; // Mutex para garantir acesso seguro
} Horario;

typedef struct
{
    char nome[BSIZE];
    Horario horarios[MAX_HORARIOS];
} Disciplina;

typedef struct
{
    int aluno_id;                   // ID do aluno
    int num_alunos;                 // Número de alunos que precisam de reservar
    char nome_pipe_resposta[BSIZE]; // Nome do pipe de resposta
} Pedido;

Disciplina disciplinas[MAX_DISCIPLINAS];

void inicializa_disciplinas()
{
    for (int i = 0; i < MAX_DISCIPLINAS; i++)
    {
        // Exemplo de inicialização das disciplinas
        sprintf(disciplinas[i].nome, "Disciplina %d", i + 1);

        for (int j = 0; j < MAX_HORARIOS; j++)
        {
            disciplinas[i].horarios[j].lugares = 10;
            disciplinas[i].horarios[j].vagas = 10;
            pthread_mutex_init(&disciplinas[i].horarios[j].mutex, NULL);
        }
    }
}

void *processa_pedido(void *arg)
{
    Pedido *msg = (Pedido *)arg;
    int total_inscritos = 0;

    for (int i = 0; i < MAX_DISCIPLINAS; i++)
    {
        for (int j = 0; j < MAX_HORARIOS; j++)
        {
            pthread_mutex_lock(&disciplinas[i].horarios[j].mutex);
            if (disciplinas[i].horarios[j].vagas > 0)
            {
                disciplinas[i].horarios[j].vagas--;
                total_inscritos++;
            }
            pthread_mutex_unlock(&disciplinas[i].horarios[j].mutex);
        }
    }

    int fd_resposta;
    if ((fd_resposta = open(msg->nome_pipe_resposta, O_WRONLY)) < 0)
    {
        perror("open (resposta)");
        pthread_exit(NULL);
    }

    char resposta[BSIZE];

    sprintf(resposta, "%d", total_inscritos);

    write(fd_resposta, resposta, strlen(resposta) + 1);
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

    inicializa_disciplinas();
    pthread_t threads[num_alunos];
    Pedido pedidos[num_alunos];

    for (int i = 0; i < num_alunos; i++)
    {
        le_pipe(fd, buf, sizeof(buf));

        if (sscanf(buf, "%d %d %s", &pedidos[i].aluno_id, &pedidos[i].num_alunos, &pedidos[i].nome_pipe_resposta) != 3)
        {
            fprintf(stderr, "Erro no pedido\n");
            close(fd);
            exit(1);
        }

        pthread_create(&threads[i], NULL, processa_pedido, &pedidos[i]);
    }

    for (int i = 0; i < num_alunos; i++)
    {
        pthread_join(threads[i], NULL);
    }

    close(fd);

    return 0;
}