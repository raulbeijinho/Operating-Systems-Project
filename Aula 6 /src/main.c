/**
 * Sistemas Operativos
 * Aplicação Thread Única
 * @file: single_thread.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Biblioteca para medir o tempo
#include <pthread.h>
#define N_DISCIPLINAS 3
#define N_SECRETARIOS 10
#define N_INSC_PER_PRODUCER 5
//

// Memória global para as disciplinas
long disciplinas[N_DISCIPLINAS];
pthread_mutex_t mutexxx;
/**
 * @brief Esta função inscreve um aluno em N_DISCIPLINAS
 * @return void
 */
void inscrever_cadeiras(void) {
    for (int i = 0; i < N_DISCIPLINAS; i++) {
        pthread_mutex_lock(&mutexxx);
        disciplinas[i] += 1;
        pthread_mutex_unlock(&mutexxx);
    }
}

/**
 * @brief Esta função faz as inscrições de um número de alunos de cada vez
 * @param void *data Um ponteiro genérico para o número de inscrições a fazer
 * @return void
 */
void *producer(void *data) {
    int no_inscricoes = *((int*)data);
    
    for (int i = 0; i < no_inscricoes; i++) {
        inscrever_cadeiras();
        sleep(1);
    }
    return NULL;

}

int main(int argc, char const *argv[]) {
    int insc_per_producer = N_INSC_PER_PRODUCER;
    struct timespec start_time, end_time;
    pthread_t th_id[10];
    pthread_mutex_init(&mutexxx, NULL);

    // Verifica se há mais de 2 argumentos
    if (argc >= 2) {
        sscanf(argv[1], "%d", &insc_per_producer);
        if (insc_per_producer <= 0 || insc_per_producer > N_INSC_PER_PRODUCER) {
            insc_per_producer = N_INSC_PER_PRODUCER;
        }
    }

    // Marcando o tempo de início (tempo de parede)
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Execução das inscrições por N_SECRETARIOS
    for (int i = 0; i < N_SECRETARIOS; i++) {
        pthread_create(&th_id[i], NULL, producer, &insc_per_producer);
        //producer(&insc_per_producer)
    }

    for(int i = 0; i < N_SECRETARIOS; i++) {
        pthread_join(th_id[i], NULL);
    }

    // Marcando o tempo de fim (tempo de parede)
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Exibindo o total de inscritos por disciplina
    for (int i = 0; i < N_DISCIPLINAS; i++) {
        printf("Total inscritos na disciplina %d: %ld\n", i, disciplinas[i]);
    }

    // Calculando o tempo total de execução
    double time_taken = (end_time.tv_sec - start_time.tv_sec) + 
                        (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("O programa demorou %f segundos a executar.\n", time_taken);

    return 0;
}
