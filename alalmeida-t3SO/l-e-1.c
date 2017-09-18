#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"

#define TRUE 1

sem_t mutex;                    // Controla a regiao critica 'rc'
sem_t db;                       // controla o acesso a base de dados
int rc = 0;                     // número de processos lendo ou querendo ler

void *reader(void *arg);
void *writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&db, 0, 1);

    pthread_t th[4];

    int i;

    //Cria threads de escritor e de leitor
    for (i = 0; i < 3; i++) {
        pthread_create(&th[i], NULL, reader, (void *) &i);
    }

    for (i = 0; i < 2; i++) {
        pthread_create(&th[i], NULL, writer, (void *) &i);
    }

    while (TRUE);
    return 0;
}

/**
 * Metodo do leitor
 * @param arg
 * @return
 */
void *reader(void *arg) {
    int i = *((int *) arg);

    while (TRUE) {
        sem_wait(&mutex);       // obtém acesso exclusivo à 'rc'
        rc++;            // inscrementa o leitor

        if (rc == 1) {          // se este for o primeiro leitor...
            sem_wait(&db);
        }

        sem_post(&mutex);       // libera o acesso exclusivo a 'rc'
        read_data_base(i);       // acesso aos dados
        sem_wait(&mutex);       // obtém acesso exclusivo a 'rc'
        rc--;            // um leitor a menos agora

        if (rc == 0) {          // se este for o último leitor
            sem_post(&db);
        }

        sem_post(&mutex);       // libera o acesso exclusivo a 'rc'
        use_data_read(i);        // região não crítica
    }
}

/**
 * Metodo do escritor
 * @param arg
 * @return
 */
void *writer(void *arg) {
    int i = *((int *) arg);

    while (TRUE) {               // repete para sempre
        think_up_data(i);        // região não crítica
        sem_wait(&db);          // obtém acesso exclusivo
        write_data_base(i);      // atualiza os dados
        sem_post(&db);          // libera o acesso exclusivo
    }
}

/**
 * Escreve leitor lendo
 * @param i
 */
void read_data_base(int i) {
    printf("Leitor %d estah lendo os dados!\n", i);
    sleep(rand() % 5);
}

/**
 * Escreve leitor esta usando os dados
 * @param i
 */
void use_data_read(int i) {
    printf("Leitor %d estah usando os dados lidos!\n", i);
    sleep(rand() % 5);
}

/**
 * Escreve escritor esta pensando no que vai escrever
 */
void think_up_data(int i) {
    printf("Escritor %d estah pensando no que escrever!\n", i);
    sleep(rand() % 5);
}

/**
 * Escreve leitor está escrevendo os dados
 * @param i
 */
void write_data_base(int i) {
    printf("Escritor %d estah escrevendo os dados!\n", i);
    sleep(rand() % 5);
}
