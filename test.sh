nasm -f macho64 sample/test.s
nasm -f macho64 sample/util.s
gcc -o test sample/test.o sample/util.o
