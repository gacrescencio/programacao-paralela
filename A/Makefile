FLAGS=-O3
EXEC=knn
CC=gcc

ALL: $(EXEC)

$(EXEC): knn.o
	$(CC) $(FLAGS) knn.o -o $(EXEC)

knn.o: knn.c knn.h
	$(CC) $(FLAGS) -c knn.c -o knn.o

# Alvo para executar e medir o tempo de execução
run: $(EXEC)
	/usr/bin/time -v ./$(EXEC) < small.in > output.txt 2> time.txt

clean:
	rm -rf $(EXEC) *.o output.txt time.txt
