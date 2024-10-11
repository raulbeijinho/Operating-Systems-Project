PIPE_NAME="/tmp/suporte"
#1. Verificar se o named pipe existe; se não existir, sair do script.


if [[ ! -p $PIPE_NAME ]]; then
    echo "Pipe $PIPE_NAME não existe. Encerrando."
    exit 1
fi

#2. Executar um ciclo em que:
while true; do

    #1. Lê do named pipe
    read pedido < $PIPE_NAME

    #2. Verifica se a mensagem recebida é "quit"; se for, sai do script    
    if [[ $pedido == "quit" ]]; then
        break
    else
        #3. Se não for “quit”, imprime qual o pedido que está a tratar
        echo "Processando pedido: $pedido"
    fi

#4. Esperar 1 segundo
    sleep 1

done




