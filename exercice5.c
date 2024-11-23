#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int n;  // Nombre de threads
sem_t *semaphores;  // Tableau de sémaphores

// Fonction exécutée par chaque thread
void* thread_function(void* arg) {
    int thread_id = *((int*)arg);  // ID du thread
    int next_thread_id = (thread_id % n) + 1;  // Thread suivant à exécuter

    while (1) {
        // Attendre son tour
        sem_wait(&semaphores[thread_id - 1]);  // Chaque thread attend son sémaphore

        // Afficher que le thread s'est exécuté
        printf("Thread %d s'exécute\n", thread_id);

        // Libérer le sémaphore du thread suivant
        sem_post(&semaphores[next_thread_id - 1]);

        // Pour simuler un travail de calcul, on peut ajouter une petite pause
        //usleep(100000);  // Pause de 0.1 seconde
        struct timespec req;
        req.tv_sec=0;
        req.tv_nsec=0;
        
        nanosleep(&req, NULL);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Vérifier que l'utilisateur a fourni le nombre de threads
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of threads>\n", argv[0]);
        return 1;
    }

    // Récupérer le nombre de threads
    n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Le nombre de threads doit être positif.\n");
        return 1;
    }

    // Initialiser les sémaphores
    semaphores = (sem_t*)malloc(n * sizeof(sem_t));
    if (semaphores == NULL) {
        perror("Erreur d'allocation mémoire pour les sémaphores");
        return 1;
    }

    // Initialiser les sémaphores pour chaque thread
    for (int i = 0; i < n; i++) {
        sem_init(&semaphores[i], 0, 0);  // Initialiser chaque sémaphore à 0 (ils attendent leur tour)
    }

    // Créer les threads
    pthread_t *threads = (pthread_t*)malloc(n * sizeof(pthread_t));
    int *thread_ids = (int*)malloc(n * sizeof(int));

    if (threads == NULL || thread_ids == NULL) {
        perror("Erreur d'allocation mémoire pour les threads");
        return 1;
    }

    // Créer chaque thread
    for (int i = 0; i < n; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }

    // Démarrer le premier thread
    sem_post(&semaphores[0]);

    // Attendre que tous les threads se terminent
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // Libérer la mémoire
    for (int i = 0; i < n; i++) {
        sem_destroy(&semaphores[i]);
    }
    free(semaphores);
    free(threads);
    free(thread_ids);

    return 0;
}



