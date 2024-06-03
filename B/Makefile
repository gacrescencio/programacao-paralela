FLAGS=-O3 -fopenmp
CC=gcc

RM=rm -f

EXEC=smooth

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) $(EXEC).c -c -o $(EXEC).o
	$(CC) $(FLAGS) $(EXEC).o -o $(EXEC)

# Alvo para executar e medir o tempo de execução
run: $(EXEC)
	/usr/bin/time -v ./$(EXEC) < image.in > output.out 2> time.out

clean:
	$(RM) $(EXEC).o $(EXEC) output.out time.out
