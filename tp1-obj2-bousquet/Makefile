CC=gcc
LIB=$(shell heptc -where)

first:
	heptc -target c first.ept;
	${CC} -I${LIB}/c main.c first_c/*.c -o first

all: first

clean:
	${RM}  *.log
	${RM}  *.epci
	${RM}  *.mls
	${RM} -r first_c
	${RM} first
