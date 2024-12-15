#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Base de dados simulada
int base_dados = 0;

// Mutex para read-write lock
pthread_rwlock_t rw_mutex;

// Função para leitores
void* lerBD(void* id) {
    int thread_id = *(int*)id;
    while (1) {
        // Bloqueio para leitura
        pthread_rwlock_rdlock(&rw_mutex);
        printf("Leitor %d: a ler o valor %d\n", thread_id, base_dados);
        sleep(1); // Simula trabalho de leitura
        pthread_rwlock_unlock(&rw_mutex);

        usleep(100000); // Pequena pausa entre leituras para dar espaco ao escreveBD
    }
    return NULL;
}

// Função para escritores
void* escreveBD(void* id) {
    int thread_id = *(int*)id;
    while (1) {
        printf("Escritor %d: tentando adquirir bloqueio de escrita\n", thread_id);
        pthread_rwlock_wrlock(&rw_mutex);
        base_dados += 1;
        printf("Escritor %d: a escrever o valor %d\n\n", thread_id, base_dados);
        sleep(2); // Simula trabalho de escrita
        pthread_rwlock_unlock(&rw_mutex);

        sleep(3); // Pequena pausa antes de tentar escrever novamente
    }
    return NULL;
}

int main() {
    // Inicializa o mutex de leitura-escrita
    pthread_rwlock_init(&rw_mutex, NULL);

    pthread_t readers[5], writers[2];
    int reader_ids[5] = {1, 2, 3, 4, 5};
    int writer_ids[2] = {1, 2};

    // Cria threads de 5 leitores
    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, lerBD, &reader_ids[i]);
    }

    // Cria threads de 2 escritores
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, escreveBD, &writer_ids[i]);
    }

    // Espera indefinidamente (podes mudar isto para testar por um período limitado)
    sleep(20);

    // Cancela as threads (opcional, se quiseres encerrar manualmente)
    for (int i = 0; i < 5; i++) {
        pthread_cancel(readers[i]);
    }
    for (int i = 0; i < 2; i++) {
        pthread_cancel(writers[i]);
    }

    // Destrói o mutex de leitura-escrita
    pthread_rwlock_destroy(&rw_mutex);

    return 0;
}
