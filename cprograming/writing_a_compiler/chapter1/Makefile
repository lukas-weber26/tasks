test: _main.o _tokenizer.o _lexer.o
	gcc -g _main.o _tokenizer.o _lexer.o -o test 
	./test ./test_input.txt
	rm _*o

_main.o: ./main.c
	gcc -g -c ./main.c -o _main.o

_tokenizer.o: ./tokenizer.c
	gcc -g -c ./tokenizer.c -o _tokenizer.o

_lexer.o: ./lexer.c
	gcc -g -c ./lexer.c -o _lexer.o

run: ./test
	./test ./test_input.txt

