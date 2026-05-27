CC=gcc
LIB=$(shell heptc -where)

counter:
	heptc -target c counter.ept;
	${CC} -I${LIB}/c main.c counter_c/*.c -o counter

all: counter

clean:
	${RM}  *.log
	${RM}  *.epci
	${RM}  *.mls
	${RM} -r counter_c
	${RM} counter
