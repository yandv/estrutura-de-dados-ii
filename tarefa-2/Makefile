all:
	mkdir -p bin
	rm -rf bin/output
	gcc -o ./bin/executable.exe src/partition-creator.c $(SORT_ROUTINE) -lm
	./bin/executable.exe $(ARGS)

creator:
	mkdir -p bin
	gcc -o ./bin/file-creator.exe src/file-creator.c -lm
	./bin/file-creator.exe $(ARGS)

hash:
	mkdir -p bin
	gcc -o ./bin/hashtable.exe src/hashtable.c -lm
	./bin/hashtable.exe $(ARGS)

clean:
	rm -rf bin