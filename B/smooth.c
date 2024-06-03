#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h> // Inclusão da biblioteca OpenMP

#define image(x,y) pixels[y*width+x]
#define smooth(x,y) filtered[y*width+x]

typedef struct {
    char red, green, blue, alpha;
} RGBA;

int main(int argc, char *argv[]) {
    FILE *in;
    FILE *out;

    in = fopen("image.in", "rb");
    if (in == NULL) {
        perror("image.in");
        exit(EXIT_FAILURE);
    }

    out = fopen("image.out", "wb");
    if (out == NULL) {
        perror("image.out");
        exit(EXIT_FAILURE);
    }

    short width, height;

    fread(&width, sizeof(width), 1, in);
    fread(&height, sizeof(height), 1, in);

    fwrite(&width, sizeof(width), 1, out);
    fwrite(&height, sizeof(height), 1, out);

    RGBA *pixels = (RGBA *) malloc(height * width * sizeof(RGBA));
    RGBA *filtered = (RGBA *) malloc(height * width * sizeof(RGBA));

    if (pixels == NULL || filtered == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int DY[] = {-2, -2, -2, -2, -2, -1, -1, -1, -1, -1, +0, +0, +0, +0, +0, +1, +1, +1, +1, +1, +2, +2, +2, +2, +2};
    int DX[] = {-2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2};

    int x, y, d, dx, dy, i;

    do {
         // Lê os pixels da imagem do arquivo de entrada
        if (!fread(pixels, height * width * sizeof(RGBA), 1, in))
            break;

        // Paralelizar o loop que itera sobre cada pixel da imagem
        #pragma omp parallel for private(x, y, d, dx, dy, i)
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                // Paralelizar o loop que itera sobre os componentes RGBA do pixel
                #pragma omp parallel for private(d, dx, dy)
                for (i = 0; i < 4; i++) { 
                    long long int sum = 0;
                    // Itera sobre cada deslocamento na janela de suavização 5x5
                    for (d = 0; d < 25; d++) {
                        dx = x + DX[d];// Calcula a posição horizontal do vizinho
                        dy = y + DY[d];// Calcula a posição vertical do vizinho
                        // Verifica se o vizinho está dentro dos limites da imagem
                        if (dx >= 0 && dx < width && dy >= 0 && dy < height) {
                             // Soma o valor do componente RGBA do vizinho
                            sum += *(((char*) (&image(dx, dy))) + i);
                        }
                    }
                     // Calcula a média dos valores dos vizinhos e armazena no pixel suavizado
                    (*(((char*) (&smooth(x, y)) + i))) = sum / 25;
                }
            }
        }

        fwrite(filtered, height * width * sizeof(RGBA), 1, out);

    } while (!feof(in));

    free(pixels);
    free(filtered);

    fclose(out);
    fclose(in);

    return EXIT_SUCCESS;
}
