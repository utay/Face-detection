#define _DEFAUTL_SOURCE

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/stat.h>       
#include <errno.h>
#include <sys/types.h>

void open_db(sqlite3 *db); 
int create_db();
int see_all_db();
int destroy_db();
int delete_id_db(char *id);
int insert(char *name, char *image);
char* cp_picture_folder(char *picture);
int insert_folder (char *folder);
people return_people_by_id(int id);

