fqvalue:main.o init.o line_opt.o note.o
	cc -o fqvalue main.o init.o line_opt.o note.o -lm
main.o:fqvalue_v2.4.c
	cc -o main.o -c fqvalue_v2.4.c
init.o:init.c
	cc -c init.c
line_opt.o:line_opt.c
	cc -c line_opt.c
note.o:note.c
	cc -c note.c
clean:
	rm main.o init.o line_opt.o note.o
