#1. Criar um named pipe com o nome /tmp/suporte
mkfifo /tmp/suporte

#2. Executar 3 students em background
./student &
./student &
./student &

#3. Executar 1 suporte_agente em background
./suporte_agente &

#4. Esperar 1 segundo (com sleep) e mandar o texto quit para o named pipe
sleep 1

#5. Esperar até todos os processos executados anteriormente terminarem
wait

#6. Remover o named pipe
rm /tmp/suporte