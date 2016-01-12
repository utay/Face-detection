CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` `pkg-config --cflags gtk+-3.0`
CFLAGS= -Wall -Wextra -std=c99 -O3 -g -D_DEFAULT_SOURCE -D_BSD_SOURCE -pedantic
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image -lm -lpthread `pkg-config --libs sqlite3` `pkg-config --libs gtk+-3.0`

SRC_ADABOOST= SDL/pixel_operations.c SDL/learn.c SDL/types.c SDL/adaboost.c SDL/sdl.c
SRC_PREDICT= SDL/predict.c SDL/adaboost.c SDL/types.c SDL/pixel_operations.c SDL/sdl.c
SRC_DATABASE= Database/BDD.c
SRC_GTK= GTK/windows.c Database/BDD.c SDL/predict.c SDL/adaboost.c SDL/types.c SDL/pixel_operations.c SDL/sdl.c
SRC_SEE_DB = GTK/see_db.c
SRC_PREDICT2= SDL/predict2.c SDL/adaboost.c SDL/types.c SDL/pixel_operations.c SDL/sdl.c
SRC_ADD_FOLDER = Database/add_folder.c

OBJ_ADABOOST= ${SRC_ADABOOST:.c=.o}
OBJ_PREDICT= ${SRC_PREDICT:.c=.o}
OBJ_DATABASE= ${SRC_DATABASE:.c=.o}
OBJ_GTK= ${SRC_GTK:.c=.o}
OBJ_SEE_DB= ${SRC_SEE_DB:.c=.o}
OBJ_PREDICT2= ${SRC_PREDICT2:.c=.o}
OBJ_ADD_FOLDER= ${SRC_ADD_FOLDER:.c=.o}


DEP_ADABOOST= ${SRC_ADABOOST:.c=.d}
DEP_PREDICT= ${SRC_PREDICT:.c=.d}
DEP_DATABASE= ${SRC_DATABASE:.c=.d}
DEP_GTK= ${SRC_GTK:.c=.d}
-include ${DEP_ADABOOST}
-include ${DEP_PREDICT}

all: SDL/learn GTK/windows GTK/see_db SDL/predict2 Database/add_folder

SDL/learn: $(OBJ_ADABOOST)
	$(CC) $(CFLAGS) $(OBJ_ADABOOST) $(LDLIBS) -o SDL/learn

SDL/predict: $(OBJ_PREDICT)
	$(CC) $(CFLAGS) $(OBJ_PREDICT) $(LDLIBS) -o SDL/predict

GTK/windows: $(OBJ_DATABASE) $(OBJ_GTK)
	$(CC) $(CFLAGS) $(OBJ_GTK) $(LDLIBS) -o GTK/windows

GTK/see_db: $(OBJ_SEE_DB)  $(OBJ_DATABASE)
	$(CC) $(CFLAGS) $(OBJ_SEE_DB) $(LDLIBS) -o GTK/see_db

SDL/predict2:$(OBJ_PREDICT2)
	$(CC) $(CFLAGS) $(OBJ_PREDICT2) $(LDLIBS) -o SDL/predict2

Database/add_folder: $(OBJ_ADD_FOLDER)
	$(CC) $(CFLAGS) $(OBJ_ADD_FOLDER) $(LDLIBS) -o Database/add_folder



clean:
	find . -name \*~ -exec rm \-fv {} \;
	find . -name \*.o -exec rm \-fv {} \;
	find . -name \*.d -exec rm \-fv {} \;
	rm -f SDL/learn SDL/predict Database/BDD GTK/windows GTK/see_db BDD/add_folder SDL/predict2 Database/add_folder


