#include <stdio.h>
#include <pthread.h>
#include <unistd.h> // Para usar sleep()

pthread_mutex_t lock;
pthread_cond_t cond;

void *thread_waiter(void *arg) {
    pthread_mutex_lock(&lock);

    // Espera até a condição ser sinalizada
    printf("Thread A: Eu sou\n");
    //pthread_cond_wait(&cond, &lock);

    // Quando chegar aqui, a condição foi sinalizada
    printf("Thread A: Mau\n");

    pthread_mutex_unlock(&lock);
    return NULL;
}

void *thread_signaler(void *arg) {
    sleep(1); // Simula algum trabalho antes de sinalizar

    pthread_mutex_lock(&lock);

    printf("Thread B: Professor Pai\n");
    // Sinaliza
    //pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Inicializa o mutex e a variável de condição
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // Cria as threads
    pthread_create(&t1, NULL, thread_waiter, NULL);
    pthread_create(&t2, NULL, thread_signaler, NULL);

    // Aguarda a conclusão das threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destrói o mutex e a variável de condição
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
