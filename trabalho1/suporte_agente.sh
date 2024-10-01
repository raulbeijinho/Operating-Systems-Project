
if [[ ! -p $PIPE_NAME ]]; then
exit 1;
fi

while true; do

    read pedido < $PIPE_NAME

    if [[ $pedido == "quit" ]]; then
        break
    fi

    echo "A tratar pedido: $pedido"

    sleep 1

done




