#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_THREADS	5

void *PrintHello(void *threadid)
{
   long tid = (long)threadid;
   int tsleep = rand()%5;
   sleep(rand()%5);
   printf("Hello World! It's me, thread #%ld, slept %d!\n", tid, tsleep);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   //identificador de threads
   pthread_t threads[MAX_THREADS];
   int rc;
   long t;
   void *res;
   int num_threads = MAX_THREADS;
   
   // não deixa exceder o numero de 5 threads
   if (argc>=2) {
      sscanf (argv[1], "%d", &num_threads);
      if (num_threads <= 0 || num_threads > MAX_THREADS)
         num_threads = MAX_THREADS;
   }
   // for para criar threads
   for(t=0;t<num_threads;t++){
     printf("In main: creating thread %ld of %d\n", t, num_threads);
     rc = pthread_create (&threads[t], NULL, PrintHello, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
      printf ("Created thread id=%d\n", threads[t]);
     }

   for(t=0; t<num_threads; t++) {
      rc = pthread_join(threads[t], &res);
      if (rc != 0) {
         perror ("pthread_join");
         exit(1);
      }
      printf ("Joined with thread id=%d, return value=%d\n", threads[t], res);
      free (res);
   }
    printf("Goodbye cruel world!\n");

   /* Last thing that main() should do */
   pthread_exit(NULL);
}