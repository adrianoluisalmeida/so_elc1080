#/bin/bash

ShowTime () {
    time_finish=0 #variavel que vai ter o tempo total das execuções
    file=alalmeida #nome do arquivo

    for (( c=1; c<=$1; c++ ))
    do
        ./$file $2 $3 > file.txt
        time=`sed -n '$p' file.txt` #pega a ultima linha do resultado (Na última linha está o tempo calculado com  a biblioteca time.h no C)
        time_finish=`echo $time + $time_finish | bc` #calcula o tempo da execução atual com as execuções anteriores
    done

    echo -e "\n"

    echo "RESULADO (Tam. Vetor $2 Num. Proc./Threads $3)"

    echo "Tempo total: " $time_finish

    echo -e "\n\n"
    
    
}

#chamada da função para execuções
#param 1 num de execuções do for para o teste
#param 2 tam. do vetor = n
#param 3 num de threads/processos = m
ShowTime 10000 30 4

#apaga arquivo
rm file.txt