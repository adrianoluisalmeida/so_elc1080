#/bin/bash

PROCESSES=5 #parametro de processos para o executavel do trabalho

#Parametro 1 da função: Total de execuções do FOR, que vai executar o código em C
#Parametro 2 da função: Qual é o tipo da estrutura que vai ser executado 1 - ARVORE / 2 - CADEIA
#Parametro 3 da função: Altura para o arquivo executavel

ShowTime () {
    time_finish=0 #variavel que vai ter o tempo total das execuções
    file=alalmeida-t1SO #nome do arquivo

    for (( c=1; c<=$1; c++ ))
    do

        ./$file $3 $2 > file.txt
        time=`sed -n '$p' file.txt` #pega a ultima linha do resultado (Na última linha está o tempo calculado com  a biblioteca time.h no C)
        time_finish=`echo $time + $time_finish | bc` #calcula o tempo da execução atual com as execuções anteriores
    done

    echo -e "\n"

    if [ ${2} -eq 1 ];
     then
        echo "RESULTADOS ESTRUTURA DO TIPO ARVORE (Altura $3)"
     else
        echo "RESULTADOS ESTRUTURA DO TIPO CADEIA (Altura $3)"
     fi

    echo "Total de execuçoes: " $1
    echo "Tempo total: " $time_finish

    echo -e "\n\n"
    
    
}

#for para testar alturas diferentes
for (( i=2; i<=7; i++ ))
    do
    #Executa o tipo Arvore
    ShowTime 30 1 $i

    #Executa o tipo Cadeia
    ShowTime 30 2 $i
done


#apaga arquivo
rm file.txt
