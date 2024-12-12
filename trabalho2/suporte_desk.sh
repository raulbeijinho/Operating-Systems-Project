# Criar um named pipe com o nome /tmp/suporte

if [[ ! -p /tmp/suporte ]]; then
    mkfifo /tmp/suporte
fi

# Obter os números máximos de alunos NALUN, disciplinas NDISCIP e lugares NLUG por sala como argumentos passados na execução do scrpt

if [ $# -ne 4 ]; then
    exit 1
fi

NALUN=$1      
NDISCIP=$2    
NLUG=$3
NSTUD=$4

# Com base em NALUN, NDISCIP e NLUG, calcular o número máximo de horários NHOR 

LUGARES_POR_DISCIPLINA=$((NDISCIP * NLUG))

NHOR=$(( (NALUN + LUGARES_POR_DISCIPLINA - 1) / LUGARES_POR_DISCIPLINA ))

# Executar 1 suport_agent em background passando como argumento NALUN

./trabalho2agent/output/main $NALUN &

# 5 

ALUNOS_POR_STUDENT=$((NALUN / NSTUD))

for (( i = 1; i <= NSTUD; i++ )); do
    
    ALUNO_INICIAL=$(( (i - 1) * ALUNOS_POR_STUDENT + 1 ))
    
    ./trabalho2student/output/main $NSTUD $ALUNO_INICIAL $ALUNOS_POR_STUDENT 

done
# esperar

wait

# remover o pipe

rm /tmp/suporte



