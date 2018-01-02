XX=g++
FLAGS=-g
TARGET=Download
LIB=-lcurl -pthread
FILESET=-D_LARGE_FILES -D _FILE_OFFSET_BITS=64

SOURCEDIR=src
PROJDIR=proj
BINDIR=bin

SOURCELIST=${wildcard ${SOURCEDIR}/*.cpp}
PROJS=${patsubst ${SOURCEDIR}/%.cpp, ${PROJDIR}/%.o, ${SOURCELIST}}
PROJDEPS=${OBJS:.o}

${BINDIR}/${TARGET}:${PROJS} ${PROJDEPS}
	${shell mkdir -p ${BINDIR}}
	${XX} ${LIB} -o ${BINDIR}/${TARGET} ${PROJS}

${PROJDIR}/%.o:${SOURCEDIR}/%.cpp
	${shell mkdir -p ${PROJDIR}}
	${XX} -c $< -o $@ ${FILESET} ${FLAGS}


-include ${PROJDEPS}

clean:
	@rm -rf ${PROJS} ${BINDIR}/${TARGET} ${PROJDEPS}
.PHONY : clean
