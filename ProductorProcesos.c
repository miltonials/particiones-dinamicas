#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>  // Agregar esta línea
#include <semaphore.h>
#include <time.h>


#define BEST_FIT 0
#define WORST_FIT 1
#define FIRST_FIT 2
#define SHM_SIZE 1024  // Tamaño de la memoria compartida

// Función para simular el tiempo de ejecución de un proceso
void simular_tiempo(int tiempo) {
    sleep(tiempo);
}

int main() {
    // Crear semáforo para controlar el acceso exclusivo al algoritmo de búsqueda de espacio
    sem_t *mutex;
    mutex = sem_open("/mutex", O_CREAT, 0666, 1);  // Aquí se usa O_CREAT
    if (mutex == SEM_FAILED) {
        perror("Error al crear semáforo");
        exit(EXIT_FAILURE);
    }

    // Solicitar tipo de algoritmo de asignación de memoria
    char algoritmo[20];
    printf("Ingrese el tipo de algoritmo de asignación de memoria (Best-Fit, First-Fit, Worst-Fit): ");
    scanf("%s", algoritmo);

    // Obtener identificador de la memoria compartida
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al solicitar memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Adjuntar la memoria compartida al espacio de direcciones del proceso
    int *memoria = (int *)shmat(shmid, NULL, 0);
    if (memoria == (int *)-1) {
        perror("Error al adjuntar la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Generar procesos de manera aleatoria
    srand(time(NULL));
    int proceso_id = 1;
    while (1) {
        // Simular un tiempo aleatorio para generar el próximo proceso
        int tiempo_creacion = rand() % 31 + 30;  // Entre 30 y 60 segundos
        sleep(tiempo_creacion);

        // Generar tamaño aleatorio para el proceso
        int tamano_proceso = rand() % 10 + 1;  // Entre 1 y 10 líneas de memoria

        // Buscar espacio en la memoria compartida para el proceso
        sem_wait(mutex);  // Iniciar región crítica
        int espacio_disponible = 0;
        for (int i = 0; i < SHM_SIZE; ++i) {
            if (memoria[i] == 0) {  // Espacio vacío
                espacio_disponible++;
                if (espacio_disponible == tamano_proceso) {
                    // Asignar espacio en la memoria para el proceso
                    for (int j = i - tamano_proceso + 1; j <= i; ++j) {
                        memoria[j] = proceso_id;
                    }
                    printf("Proceso %d asignado a memoria.\n", proceso_id);
                    break;
                }
            } else {
                espacio_disponible = 0;  // Reiniciar contador
            }
        }
        sem_post(mutex);  // Finalizar región crítica

        if (espacio_disponible < tamano_proceso) {
            printf("No hay suficiente espacio en la memoria para el proceso %d. Muriendo...\n", proceso_id);
            break;  // Si no hay suficiente espacio, terminar el programa
        }

        // Simular tiempo de ejecución del proceso
        int tiempo_ejecucion = rand() % 41 + 20;  // Entre 20 y 60 segundos
        simular_tiempo(tiempo_ejecucion);

        // Liberar espacio en la memoria compartida
        sem_wait(mutex);  // Iniciar región crítica
        for (int i = 0; i < SHM_SIZE; ++i) {
            if (memoria[i] == proceso_id) {
                memoria[i] = 0;  // Liberar espacio
            }
        }
        sem_post(mutex);  // Finalizar región crítica

        proceso_id++;
    }

    // Liberar la memoria compartida
    if (shmdt(memoria) == -1) {
        perror("Error al liberar la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Eliminar el semáforo
    if (sem_close(mutex) == -1) {
        perror("Error al cerrar el semáforo");
        exit(EXIT_FAILURE);
    }

    // Informar la finalización del programa
    printf("Programa Productor de Procesos terminado.\n");

    return 0;
}
