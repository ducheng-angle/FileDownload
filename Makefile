XX=g++
FLAGS=-g
TARGET=Download
LIB=-lcurl -pthread

SOURCEDIR=src
PROJDIR=proj
BINDIR=bin

SOURCELIST=${wildcard ${SOURCEDIR}/*.cpp}
PROJS=${patsubst ${SOURCEDIR}/%.cpp, ${PROJDIR}/%.o, ${SOURCELIST}

${BINDIR}/${TARGET}:${PROJS} 
	${shell mkdir -p ${BINDIR}}
	${XX} ${LIB} -o ${BINDIR}/${TARGET} ${PROJS}

${PROJDIR}/%.o:${SOURCEDIR}/%.cpp
	${shell mkdir -p ${PROJDIR}}
	${XX} -c $< -o $@ ${FLAGS} 

clean:
	@rm -rf ${PROJS} ${BINDIR}/${TARGET}
.PHONY : clean
