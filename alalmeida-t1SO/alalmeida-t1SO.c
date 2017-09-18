/* arquivo ex6-fork-wait.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>


//Entradas
// Parametro 1 - Determina os processos
// Parametro 2 - Tipo da estrutura (1 - Árvore / 2 - Cadeia)

double calTime(struct timespec t0, struct timespec t1){
    return ((double)t1.tv_sec - t0.tv_sec) + ((double)(t1.tv_nsec-t0.tv_nsec) * 1e-9);
}

void estrutura(int entrada, int arg2) {
    pid_t idProcesso;
    int estado;

    for (int i = 0; i < entrada; ++i) {

        idProcesso = fork();

        if (idProcesso < 0) {
            exit(errno);
        }

        if (arg2 == 2) { //Caso for a estrutura de cadeia
            if (idProcesso == 0) { //FILHO
                printf("Sou o filho (%d), meu pai eh %d\n", getpid(), getppid());
                continue;
            }
        } else { //Caso for a estrutura de arvore
            if (idProcesso == 0 || fork() == 0) { //FILHO
                printf("Sou o filho (%d), meu pai eh %d\n", getpid(), getppid());
                continue;

            }
        }

        if (idProcesso != 0) { //PAI
            printf("Sou o processo pai (%d)\n", getpid());
            wait(&estado); //Espera pelos filhos
        }
        break;
    }
    wait(&estado);

    printf("Sou o processo (%d) e estou saindo.\n", getpid());
}

int main(int argc, char *argv[]) {

int principal = getpid();
    int h = 0, arg1 = 0, arg2 = 0;
    
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0); //inicio da contagem de tempo
    
    //Testa se o primeiro argumento foi informado
    if (argv[1] == 0) {
        printf("Primeiro argumento não informado.\n");
        exit(0);
    } else if (argv[2] == 0) {
        printf("Segundo argumento não informado ou inválido.\n(1-arvore/2-cadeia)\n");
        exit(0);
    } else {
        arg1 = strtol(argv[1], NULL, 10);
        arg2 = strtol(argv[2], NULL, 10);
    }

    //Testa o argumento de entrada
    if (arg1 < 1) {
        printf("Argumento inválido\n");
        exit(0);
    }

	printf("%d", arg2);

    if(arg2 < 1 || arg2 > 2){
        printf("Segundo argumento não informado ou inválido.\n(1-arvore/2-cadeia)\n");
        exit(0);
    }


    h = pow(2, (arg1 + 1)) - 1; //Calculo do Total de processos

    if (arg2 == 1)
        estrutura(arg1, arg2);
    else
        estrutura(h, arg2);

    if (principal == getpid()) { //Verifica se o processo que vai ser finalizado é pai superior
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
        
        printf("Tempo total (segundos):\n%lf\n", calTime(t0, t1));
        
    }

    exit(0);
}
