PIPE_NAME="/tmp/suporte"

if [[ ! -p $PIPE_NAME ]]; then
mkfifo $PIPE_NAME
fi

for i in {1..3}; do
 
 ./student "pedido $i"

done

./student_agente.sh 

sleep 1

echo "quit" > $PIPE_NAME

wait

rm $PIPE_NAME
