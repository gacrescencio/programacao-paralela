#include "knn.h"

char knn(int n_groups, Group * groups, int k, Point to_evaluate) {
    char * labels = (char *) malloc(sizeof(char) * k);
    float * distances = (float *) malloc(sizeof(float) * k);

    int i, j, x, y;

    for (i = 0; i < k; i++) {
        labels[i] = -1;
        distances[i] = -1;
    }
    //Paraleliza o loop que percorre todos os grupos e mede as distâncias
    #pragma omp parallel for private(i, j, x, y) shared(labels, distances)
    for (i = 0; i < n_groups; i++) {
        Group g = groups[i];
        for (j = 0; j < g.length; j++) {
            float d = euclidean_distance_no_sqrt(to_evaluate, g.points[j]);
            // utilizado o omp critical para evitar condição de corrida, uma vez que as variaveis diastances e labels são compartilhadas
            #pragma omp critical
            {
            // Insere a nova distância e rótulo na posição correta nos arrays
            for (x = 0; x < k; x++) {
                if (d < distances[x] || distances[x] == -1) {
                    for (y = k - 1; y > x; y--) {
                        distances[y] = distances[y - 1];
                        labels[y] = labels[y - 1];
                    }
                    distances[x] = d;
                    labels[x] = g.label;
                    break;
                }
            }
        }
        
    }
    }
    
    qsort(labels, k, sizeof(char), compare_for_sort);

    char most_frequent = labels[0];
    int most_frequent_count = 1;
    int current_frequency = 1;

    for (i = 1; i < k; i++) {
        if (labels[i] != labels[i - 1]) {
            if (current_frequency > most_frequent_count) {
                most_frequent = labels[i - 1];
                most_frequent_count = current_frequency;
            }

            current_frequency = 1;
        } else {
            current_frequency++;
        }

        if (i == k - 1 && current_frequency > most_frequent_count) {
            most_frequent = labels[i - 1];
            most_frequent_count = current_frequency;
        }
    }


    return most_frequent;
}

int main() {
    int n_groups = parse_number_of_groups();
    
    Group * groups = (Group *) malloc(sizeof(Group) * n_groups);

    // Paraleliza a leitura e inicialização dos grupos
    #pragma omp parallel for
    for (int i = 0; i < n_groups; i++) {
        groups[i] = parse_next_group();
    }

    int k = parse_k();

    Point to_evaluate = parse_point();

    printf("%c", knn(n_groups, groups, k, to_evaluate));
}