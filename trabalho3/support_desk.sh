# Criar um named pipe com o nome /tmp/suporte

if [[ ! -p /tmp/suporte ]]; then
    mkfifo /tmp/suporte
fi

#criar pipe admin

if [[ ! -p /tmp/admin ]]; then
    mkfifo /tmp/admin
fi

# Obter os números máximos de alunos NALUN, disciplinas NDISCIP e lugares NLUG por sala como argumentos passados na execução do scrpt

if [ $# -ne 2 ]; then
    exit 1
fi

NALUN=$1      
NSTUD=$2

# Executar 1 suport_agent em background passando como argumento NALUN

./trabalho3agent/output/main $NALUN &

# 5 

ALUNOS_POR_STUDENT=$((NALUN / NSTUD))

for (( i = 1; i <= NSTUD; i++ )); do
    
    ALUNO_INICIAL=$(( (i - 1) * ALUNOS_POR_STUDENT + 1 ))
    
    ./trabalho3student/output/main $NSTUD $ALUNO_INICIAL $ALUNOS_POR_STUDENT 

done
# esperar

wait

# remover o pipe

rm /tmp/suporte

rm /tmp/admin

for ((i = 1; i <= NSTUD; i++)); do

rm /tmp/student_$i

done
