FLAGS=-O3
CC=gcc

RM=rm -f

EXEC=smooth

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) $(EXEC).c -c -o $(EXEC).o
	$(CC) $(FLAGS) $(EXEC).o -o $(EXEC)

clean:
	$(RM) $(EXEC).o $(EXEC)
