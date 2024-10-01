PIPE_NAME="/tmp/suporte"

if [[ ! -p $PIPE_NAME ]]; then
    echo "Pipe $PIPE_NAME não existe. Encerrando."
    exit 1
fi

while true; do

    read pedido < $PIPE_NAME

    if [[ $pedido == "quit" ]]; then
        break
    fi

    sleep 1

done




