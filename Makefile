build: 
	gcc -m32 -std=c99 main1.c Node.c Queue.c Stack.c -o tema2
run:
	./a.out
clean:
	rm -rf a.out