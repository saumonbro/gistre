CC=gcc
LIB=$(shell heptc -where)
SRC=extern.c

EPI=extern.epi
EPCI=$(EPI:.epi=.epci)


all: main

scheduler_data:
	heptc -target c scheduler_data.ept

extern: extern.epi scheduler_data
	heptc -c $<

main: extern scheduler_data
	heptc -target c scheduler.ept
	${CC} -I${LIB}/c -I. ${SRC} -Ischeduler_data_c main.c scheduler_data_c/*.c scheduler_c/*.c -o main

clean:
	${RM}  *.log
	${RM}  *.epci
	${RM}  *.mls
	${RM} -r scheduler_data_c
	${RM} -r scheduler_c
	${RM} main
