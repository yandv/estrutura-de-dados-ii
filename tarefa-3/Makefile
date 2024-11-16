all:
	mkdir -p bin
	rm -rf bin/output
	gcc -o ./bin/executable.exe src/lefkowitz.c -lm
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