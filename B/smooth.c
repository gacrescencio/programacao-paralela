#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Macros para acessar os elementos das matrizes 2D armazenadas em arrays 1D
#define image(x,y) pixels[y*width+x]
#define smooth(x,y) filtered[y*width+x]

// Estrutura RGBA para representar um pixel
typedef struct {
    char red, green, blue, alpha;
} RGBA;

int main(int argc, char *argv[]) {
    FILE *in; // Ponteiro para o arquivo de entrada
    FILE *out; // Ponteiro para o arquivo de saída

    // Abre o arquivo de entrada em modo de leitura binária
    in = fopen("image.in", "rb");
    if (in == NULL) {
        perror("image.in"); // Exibe mensagem de erro se não conseguir abrir o arquivo
        exit(EXIT_FAILURE); // Termina o programa com falha
    }

    // Abre o arquivo de saída em modo de escrita binária
    out = fopen("image.out", "wb");
    if (out == NULL) {
        perror("image.out"); // Exibe mensagem de erro se não conseguir abrir o arquivo
        exit(EXIT_FAILURE); // Termina o programa com falha
    }

    short width, height; // Variáveis para armazenar a largura e altura da imagem

    // Lê a largura e altura da imagem do arquivo de entrada
    fread(&width, sizeof(width), 1, in);
    fread(&height, sizeof(height), 1, in);

    // Grava a largura e altura da imagem no arquivo de saída
    fwrite(&width, sizeof(width), 1, out);
    fwrite(&height, sizeof(height), 1, out);

    // Aloca memória para os pixels da imagem original e suavizada
    RGBA *pixels = (RGBA *) malloc(height * width * sizeof(RGBA));
    RGBA *filtered = (RGBA *) malloc(height * width * sizeof(RGBA));

    // Verifica se a alocação de memória foi bem-sucedida
    if (pixels == NULL || filtered == NULL) {
        perror("malloc"); // Exibe mensagem de erro se a alocação falhar
        exit(EXIT_FAILURE); // Termina o programa com falha
    }

    // Deslocamentos para os vizinhos na janela de suavização 5x5
    int DY[] = {-2, -2, -2, -2, -2, -1, -1, -1, -1, -1, +0, +0, +0, +0, +0, +1, +1, +1, +1, +1, +2, +2, +2, +2, +2};
    int DX[] = {-2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2, -2, -1, +0, +1, +2};

    int x, y, d, dx, dy, i; // Variáveis de controle para os loops

    do {
        // Lê os pixels da imagem do arquivo de entrada
        if (!fread(pixels, height * width * sizeof(RGBA), 1, in))
            break; // Sai do loop se a leitura falhar

        // Itera sobre cada pixel da imagem
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                // Itera sobre cada componente RGBA do pixel
                #pragma omp parallel for private(x, y, d, dx, dy, i)
                for (i = 0; i < 4; i++) {
                    long long int sum = 0; // Variável para acumular a soma dos valores dos vizinhos

                    // Itera sobre cada deslocamento na janela de suavização 5x5
                    for (d = 0; d < 25; d++) {
                        dx = x + DX[d]; // Calcula a posição horizontal do vizinho
                        dy = y + DY[d]; // Calcula a posição vertical do vizinho

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

        // Grava os pixels suavizados no arquivo de saída
        fwrite(filtered, height * width * sizeof(RGBA), 1, out);

    } while (!feof(in)); // Continua até o final do arquivo de entrada

    // Libera a memória alocada para os pixels
    free(pixels);
    free(filtered);

    // Fecha os arquivos de entrada e saída
    fclose(out);
    fclose(in);

    return EXIT_SUCCESS; // Termina o programa com sucesso
}
