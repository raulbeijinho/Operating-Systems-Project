# Criar um named pipe com o nome /tmp/suporte

if [[ ! -p /tmp/support_pipe ]]; then
    mkfifo /tmp/support_pipe
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

./suport_agent $NALUN &

# 5 

ALUNOS_POR_STUDENT=$((NALUN / NSTUD))

./student $i $ALUNOS_POR_STUDENT

# esperar

wait

# remover o pipe

rm /tmp/suporte



