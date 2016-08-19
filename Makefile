
all:
	gcc -o cgit-gsub main.c -I/usr/local/include -L/usr/local/lib -lpcre

clean:
	rm -rf cgit-gsub
