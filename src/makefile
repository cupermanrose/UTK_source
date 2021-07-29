CC = g++
CFLAGS = -w -std=c++11 
OBJS = cellTree.o collection.o filemem.o global.o main.o param.o point.o queryGen.o rnode.o rtree.o skyline.o tgs.o utk.o virtualRNode.o rentry.o liblpsolve55.a hypercube.cpp 
INCLUDES = -I "./headers/" 
LIBS = 
FOLDER = ../bin/

main: ${OBJS} ${FOLDER}
	${CC} ${CFLAGS} ${INCLUDES} -o ../bin/utk ${OBJS}  ${LIBS} ;\
	make clean; clear; \
	echo "Successfully compiled, go to UTK/bin/ to find out!"

clean: 
	rm -f *.o

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

${FOLDER}:
	mkdir ${FOLDER}

liblpsolve55.a:
	sh download_lp_solve.sh
