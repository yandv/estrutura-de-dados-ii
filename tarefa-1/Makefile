sort = select

ifeq ($(sort), select)
	OBJS = src/sort/select-sort.c
else ifeq ($(sort), natural)
	OBJS = src/sort/natural-sort.c
else
	$(error "O algoritmo de sort deve ser 'select' ou 'natural'")
endif

all:
	mkdir -p bin
	gcc -o ./bin/executable.exe src/main.c $(OBJS) -lm
	./bin/executable.exe $(ARGS)

creator:
	mkdir -p bin
	gcc -o ./bin/creator.exe src/creator.c -lm
	./bin/creator.exe $(ARGS)

clean:
	rm -rf bin