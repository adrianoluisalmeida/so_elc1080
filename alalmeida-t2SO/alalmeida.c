#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

#define MAX 100000000
#define MIN 1


// Variáveis globais para armazenar o tamanho do vector e o n. de threads/processos
int n = 0;
int m = 0;

//Função para testar os parâmetros de entrada
int testInput(int argc, char **argv);

//Função para testar se o numero eh primo
bool test_primo(int n);

//Função para testar se o numero eh primo
int test_total_primo(int ini, int fim, int *vector);

void *Thread(void *arg);

//Função para calcular o Tempo de exec.
double calTime(struct timespec t0, struct timespec t1) {
    return ((double) t1.tv_sec - t0.tv_sec) + ((double) (t1.tv_nsec - t0.tv_nsec) * 1e-9);
}

struct Argumentos {
    int ini;
    int fim;
    int result;
    int *vet;
};

//Função testa conta primo sequencial
void sequencial(int *vector) {

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0); //inicio da contagem de tempo

    int i, total = 0;

    total = test_total_primo(0, n, vector);

    printf("\n\nSequencial -> %d numeros primos!\n", total);

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    printf("Tempo total (segundos):\n%lf\n\n", calTime(t0, t1));


}

void funcThread(int *vector) {

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0); //inicio da contagem de tempo

    pthread_t threads[m];
    struct Argumentos args[m];

    int ini = 0, fim = 0, element = 0;

    //Total de elementos por thread
    element = n / m;


    printf("\n");

    for (int i = 0; i < m; i++) {

        ini = i * element;
        fim = ini + element;

        args[i].ini = ini;
        args[i].fim = fim;
        args[i].vet = vector;
        args[i].result = 0;

        pthread_create(&threads[i], NULL, Thread, (void *) &args[i]);
    }

    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d numeros primos %d\n", i, args[i].result);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    printf("Tempo total (segundos):\n%lf\n\n", calTime(t0, t1));


}


void *Thread(void *arg) {
    struct Argumentos *args = (struct Argumentos *) arg;

    //Armazena o total de primos na thread atual, em result
    args->result = test_total_primo(args->ini, args->fim, args->vet);
    pthread_exit(NULL);
}

void processos(int *vector) {


    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0); //inicio da contagem de tempo

    pid_t var;
    int element = 0;
    int ini = 0, fim = 0;
    int i, pid;

    //Pid do processo default
    var = getpid();

    //Total de valores por processo
    element = n / m;

    //Pipe matriz
    int fd[m][2];


    typedef struct {
        int total;
        int pid;
    } Str;

    for (i = 0; i < m; i++) {

        if (pipe(fd[i]) < 0) {
            perror("Error ao criar o pipe");
            exit(-1);
        }

        if (getpid() == var) {
            pid = fork();//inicio e filho 

            if (pid < 0) {
                printf("Error");
                exit(1);
            } else if (pid == 0) {

                ini = element * i;
                fim = ini + element;

                Str s;
                s.pid = getpid();
                s.total = test_total_primo(ini, fim, vector);

                close(fd[i][0]);
                write(fd[i][1], &s, sizeof(Str));

                exit(0);
            } else {
                //wait(NULL);

                close(fd[i][1]);
                Str s;
                read(fd[i][0], &s, sizeof(Str));

                printf("processo PID %d -> %d numeros primos!\n", s.pid, s.total);

            }

        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    printf("Tempo total (segundos):\n%lf\n\n", calTime(t0, t1));


}


int main(int argc, char **argv) {
    int *vector;

    /* Testa argumentos de entrada */
    if (!testInput(argc, argv))
        return -1;

    /* aloco espaço suficiente para o vector de n valores */
    if (!(vector = (int *) malloc(n * sizeof(int)))) {
        printf("Não foi possível alocar o vector\n");
        exit(0);
    }

    //Gera vetor randomico
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        vector[i] = (rand() % 51);
        printf(" %d ", vector[i]);
    }

    sequencial(vector);
    processos(vector);
    funcThread(vector);

    /* desaloco o vector */
    free(vector);

    return 1;
}

/*
 * Verifica se um número é primo
 */
bool test_primo(int n) {
    int i;

    if (n <= 1)
        return false;

    for (i = 2; i <= sqrt(n); i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

//Percorre o vetor e retorna o total de primos encontrados
int test_total_primo(int ini, int fim, int *vector) {

    int total = 0;

    for (int i = ini; i < fim; i++) {
        if (test_primo(vector[i])) {
            total += 1;
        }
    }

    return total;
}

//Teste dos parâmetros de entrada, se foram válidos
int testInput(int argc, char **argv) {
    if (argc != 3) {
        printf("Informe o tamanho do vector e o numero de processos/threads!\n");
        return 0;
    } else {
        n = atoi(argv[1]);
        m = atoi(argv[2]);

        if (n < MIN || n > MAX) {
            printf("O tamanho do vector deve estar entre %d e %d!\n", MIN, MAX);
            return 0;
        } else if (m < MIN || m > MAX) {
            printf("O numero de processos/threads devem estar entre  %d e %d!\n", MIN, MAX);
            return 0;
        }
    }
    return 1;
}
