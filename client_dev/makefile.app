CC=/usr/bin/x86_64-linux-gnu-gcc
OBJ=dev_app
SRC=dev_app.c
FLAGS=o
INC_FLAGS=
LIB_FLAGS=

$(OBJ):$(SRC)
	$(CC) -$(FLAGS) $(OBJ) $(SRC)
