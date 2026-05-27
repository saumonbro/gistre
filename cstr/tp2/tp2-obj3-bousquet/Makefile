CC=gcc
LIB=$(shell heptc -where)
SRC=extern.c

EPI=extern.epi
EPCI=$(EPI:.epi=.epci)

%.epci: %.epi
	heptc -c $<

main: ${EPCI}
	heptc -target c main.ept
	${CC} -I${LIB}/c -I. ${SRC} main.c main_c/*.c -o main

all: main

clean:
	${RM}  *.log
	${RM}  *.epci
	${RM}  *.mls
	${RM} -r main_c
	${RM} main
