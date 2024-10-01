PIPE_NAME="/tmp/suporte"

mkfifo $PIPE_NAME

for i in {1..3}; do
 
 ./main "pedido $i"
 
 echo "pedido $i feito$"

done

./suporte_agente.sh

sleep 1

echo "quit" > $PIPE_NAME

wait

rm $PIPE_NAME
