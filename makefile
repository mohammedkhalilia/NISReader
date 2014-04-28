CC=cc
CFLAGS=-c -D_FILE_OFFSET_BITS=64 -I/usr/include/mysql -include "header.h"
LDFLAGS=-L/usr/lib/mysql -lmysqlclient
OBJECTS=nisdata.o data_functions.o general_functions.o load_to_db.o db.o patient_dist.o drg_occurrence.o
EXECUTABLE=nis

${EXECUTABLE}:${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${LDFLAGS}

%.o:%.c
	${CC} ${CFLAGS} $<
	
clean:
	@echo "Clean Files"
	rm -f ${OBJECTS} ${EXECUTABLE}
