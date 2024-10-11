#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
   
    char pedido[100];

    strcopy(pedido, argv[1]);

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pedido>\n", argv[0]);
        return 1;
    }

    const char *pipe_name = "/tmp/suporte";
    
    int fd = open(pipe_name, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o named pipe\n");
        return 1;
    }

    
    if (write(fd, pedido, strlen(pedido)) == -1) {
        perror("Erro ao escrever no pipe\n");
        close(fd);
        return 1;
    }

    close(fd);
    
    printf("Pedido '%s' enviado com sucesso.\n", pedido);
    return 0;
}

