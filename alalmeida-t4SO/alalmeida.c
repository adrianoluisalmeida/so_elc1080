#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "stdbool.h"

//Minimo de ingressos
#define MIN 1

//Semaforos
sem_t selecionaIngresso; //Semaforo que controla a seleção de ingresso]
sem_t venda;             //Semaforo que controla a venda
sem_t semVolta;          //Comprador volta pra fila
sem_t verIngressosDisp;

typedef struct lugar {
    int num;
    bool livre;
    sem_t semRes;
} Lugar;

Lugar *lugares;


//Variaveis sobre as vendas
int numIngressos = 0;
int lugaresDisponiveis = 0;
int compradores = 0; //N de threads tmb
int verifica_cartao();
int falhasCartao = 0;
int compradoresSemIngresso = 0;
int compradoresParam = -1; //Caso seja informado por parametro no terminal

//Fila de espectadore
struct Fila {
    int capacidade;
    int *espectadores;
    int primeiro;
    int ultimo;
    int numEsp;
};

struct Fila filaEspera;

//Metodos de manipulação da fila de espectadores
void criarFila(struct Fila *f, int c);
int remover(struct Fila *f);
void inserir(struct Fila *f, int v);
void mostrarFila(struct Fila *f);

//Metodos de manipulacao da venda
void *compra(void *v);
int retornaLugarEscolido();

//Metodos de verificacao
int testInput(int argc, char **argv);
void imprimeIngressos();

int main(int argc, char **argv) {

    sem_init(&selecionaIngresso, 0, 1);
    sem_init(&semVolta, 0, 1);
    sem_init(&venda, 0, 1);
    sem_init(&verIngressosDisp, 0, 1);

    /* Testa argumentos de entrada */
    if (!testInput(argc, argv))
        return -1;

    lugares = (Lugar *) malloc(numIngressos * sizeof(Lugar));

    lugaresDisponiveis = numIngressos;

    //Total de threads 150% do numero de ingressos
    if(compradoresParam > 0)
        compradores = compradoresParam;
    else
        compradores = numIngressos * 1.5;

    //Threads
    pthread_t th[compradores];

    //Cria a fila de espectadores
    criarFila(&filaEspera, compradores);

    //Defini os lugares como vazios
    for (int i = 0; i < numIngressos; i++) {
        lugares[i].num = i + 1;
        lugares[i].livre = true;
        sem_init(&lugares[i].semRes, 0, 1);
    }

    //Insere os compradores na fila
    for (int i = 0; i < compradores; i++) {
        //Cria fila
        inserir(&filaEspera, i);
    }

    //Cria processo de compra
    for (int i = 0; i < compradores; i++) {
        pthread_create(&th[i], NULL, compra, (void *) &i);
    }

    for (int i = 0; i < compradores; i++) {
        pthread_join(th[i], NULL);
    }


    printf("\n\nFalhas compra com cartão: %d\n", falhasCartao);
    printf("Espectadores que ficaram sem ingresso (Obs.: Fora aqueles que tiveram o cartão recusado): %d\n", filaEspera.numEsp);
    printf("Ingressos não vendidos: %d\n", lugaresDisponiveis);

}
/**
 * Método principal de compra do ingresso
*/
 void *compra(void *v) {
    int index, num;

    while (lugaresDisponiveis > 0) {
        sem_wait(&selecionaIngresso);

        if (filaEspera.numEsp > 0) {
            num = remover(&filaEspera);
            index = retornaLugarEscolido();
            printf("[Espectador Nº: %d] Selecionando ingresso Nº %d...\n", num+1, lugares[index].num);
            sleep(rand() % 5);
            sem_post(&selecionaIngresso);
        } else {
            sem_post(&selecionaIngresso);
            break;
        }


        sem_wait(&lugares[index].semRes);

        if (lugares[index].livre) {

            int cartao = verifica_cartao();

            if (cartao) {
                sem_wait(&venda);
                printf("[Espectador Nº: %d] comprou ingresso Nº %d...\n", num+1, lugares[index].num);
                sleep(rand() % 5);
                lugares[index].livre = false;
                lugaresDisponiveis--;
                imprimeIngressos();
                sem_post(&venda);
            } else {
                falhasCartao++;
                printf("[Espectador Nº: %d] Cartão recusado ! Compra do ingresso Nº %d cancelada...\n", num,
                       lugares[index].num);
                sleep(rand() % 5);
            }
        } else {
            sem_wait(&semVolta);
            printf("[Espectador Nº: %d] Ingresso (%d) indisponivel..\n", num+1, lugares[index].num);
            sleep(rand() % 5);
            inserir(&filaEspera, num);
            sem_post(&semVolta);
        }
        sem_post(&lugares[index].semRes);

    }
}

/**
 * Cria a fila de espectadores
 * @param f
 * @param c
 */
void criarFila(struct Fila *f, int c) {

    f->capacidade = c;
    f->espectadores = (int *) malloc(f->capacidade * sizeof(int));
    f->primeiro = 0;
    f->ultimo = -1;
    f->numEsp = 0;


}

/**
 * Remove o primeiro da fila e retorna seu numero
 * @param f
 * @return
 */
int remover(struct Fila *f) { // pega o item do comeÃ§o da fila

    int temp = f->espectadores[f->primeiro++]; // pega o valor e incrementa o primeiro

    if (f->primeiro == f->capacidade)
        f->primeiro = 0;

    f->numEsp--;  // um item retirado
    return temp;

}

/**
 * Insere espactador na fila
 * @param f
 * @param v
 */
void inserir(struct Fila *f, int v) {

    if (f->ultimo == f->capacidade - 1)
        f->ultimo = -1;

    f->ultimo++;
    f->espectadores[f->ultimo] = v; // incrementa ultimo e insere
    f->numEsp++; // mais um item inserido

}

/**
 * Mostra a fila de espectadores
 * @param f
 */
void mostrarFila(struct Fila *f) {

    int cont, i;
    for (cont = 0, i = f->primeiro; cont < f->numEsp; cont++) {
        printf("%d\t", f->espectadores[i++]);
        if (i == f->capacidade)
            i = 0;
    }
    printf("\n\n");

}


/**
 * Retorna a posição do array principal de lugares
 * @return
 */
int retornaLugarEscolido() {
    int count = 0, num = 0;

    if (lugaresDisponiveis > 0) {
        srand(time(NULL));
        num = (rand() % lugaresDisponiveis);

        for (int i = 0; i < numIngressos; i++) {

            if (lugares[i].livre)
                count++;

            if (count == (num + 1))
                return i;
        }
    }
    return 0;

}


/**
 * Retorna se o cartao foi ou não aprovado
 * 0 = recusado
 * 1 = aprovado
 */

int verifica_cartao() {

    int count = 0, num = 0;

    srand(time(NULL));
    num = (rand() % 3);


    return num != 0;
}

/**
 * Teste dos parâmetros de entrada, se sao validos
 */
int testInput(int argc, char **argv) {

    if (argc < 2) {
        printf("Informe o nº de ingressos!\n");
        return 0;
    } else {
        numIngressos = atoi(argv[1]);
        if(argc == 3){
            compradoresParam = atoi(argv[2]);
            if (compradoresParam < MIN) {
                printf("O nº de espectadores deve ser no minimo %d!\n", MIN);
                return 0;
            }
        }


        if (numIngressos < MIN) {
            printf("O nº de ingressos deve ser no minimo %d!\n", MIN);
            return 0;
        }

    }
    return 1;
}

void imprimeIngressos() {

    printf("Ingressos disponiveis: [");
    for (int i = 0; i < numIngressos; i++) {
        if (lugares[i].livre)
            printf(" %d ", lugares[i].num);
    }
    printf("]\n");

}