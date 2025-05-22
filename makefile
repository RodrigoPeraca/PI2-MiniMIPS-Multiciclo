all:
				gcc arq.c -c -g
				gcc mainMC.c arq.o -o exec -g
clear:
				rm -f *.o
				rm -f exec
