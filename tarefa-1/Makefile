sort = substitution

ifeq ($(sort), substitution)
	SORT_ROUTINE = src/sort/substitution-sort.c
else ifeq ($(sort), natural)
	SORT_ROUTINE = src/sort/natural-sort.c
else
	$(error "O algoritmo de sort deve ser 'substitution' ou 'natural'")
endif

interpolation = winner-tree

ifeq ($(interpolation), winner-tree)
	INTERPOLATION_ROUTINE = src/interpolation/winner-tree.c
else ifeq ($(interpolation), optimal)
	INTERPOLATION_ROUTINE = src/interpolation/optimal.c
else
	$(error "O algoritmo de interpolação deve ser 'winner-tree' ou 'optimal'")
endif

all:
	mkdir -p bin
	rm -rf bin/output
	gcc -o ./bin/executable.exe src/partition-creator.c $(SORT_ROUTINE) -lm
	./bin/executable.exe $(ARGS)

creator:
	mkdir -p bin
	gcc -o ./bin/file-creator.exe src/file-creator.c -lm
	./bin/file-creator.exe $(ARGS)

interpolation:
	mkdir -p bin
	gcc -o ./bin/interpolation.exe src/interpolation.c $(INTERPOLATION_ROUTINE) -lm
	./bin/interpolation.exe $(ARGS)

reader:
	mkdir -p bin
	gcc -o ./bin/reader.exe src/partition-reader.c -lm
	./bin/reader.exe $(ARGS)

clean:
	rm -rf bin