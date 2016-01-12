//By dexemp_f - 2015/10/12
typedef struct people people;
struct people
{
    int exist;
    int id;
    char* name;
    char* picture;
} ;

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



//int nb_people = 100;
people table_people[100];
people tmp;


people create_people(int id, char* name, char* picture)
{
    people this;
    this.exist = 1;
    this.id = id;
    this.name = name;
    this.picture = picture;

    return this;
}

//it's for seealldb
static int callback(void *donnee, int nombre_colonne, char **champs, char **colones)
{
    int i;
    if (donnee != NULL)
        fprintf(stderr, "%s: ", (const char*)donnee);
    for (i = 0; i < nombre_colonne; i++)
    {
        printf("%s = %s\n", colones[i], champs[i] ? champs[i] : "NULL");
    }
    printf("\n");
    return 0;
}

/*static int callback_people (void *donnee, int nombre_colonne, char **champs, char **colones)
{
    people tmp = create_people(atoi(champs[0] ? champs[0] : "NULL"), champs[1] ? champs[1] : "NULL", champs[2] ? champs[2] : "NULL");
    int i;
    for (i = 0; table_people[i].exist != 0; i++)
    {
        //Nothing
    }
    table_people[i] = tmp;
    return 0;
}*/

/*static int callback_one_people(void *donnee, int nombre_colonne, char **champs, char **colones)
{
    tmp = create_people(atoi(champs[0] ? champs[0] : "NULL"), champs[1] ? champs[1] : "NULL", champs[2] ? champs[2] : "NULL");

    return 0;
}*/

void open_db(sqlite3 *db)
{
    int rc;

    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }
}

int create_db()
{
    char *sql = NULL;
    sqlite3 *db = NULL;
    int rc = NULL;
    char *zErrMsg = 0;

    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE FACIFICATOR ("  \
          "ID             INTEGER PRIMARY KEY AUTOINCREMENT," \
          "NAME           TEXT    NOT NULL," \
          "PICTURE        TEXT    NOT NULL" \
          ");";


    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Table created successfully\n\n");
    }
    sqlite3_close(db);
    return 0;
}

int see_all_db()
{
    create_db();
    char *zErrMsg = 0;
    char *sql;
    sqlite3 *db = NULL;
    int rc;

    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "SELECT * from FACIFICATOR";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return 0;
}

/*people* return_all_people()
{
    create_db();
    char *zErrMsg = 0;
    char *sql;
    sqlite3 *db = NULL;
    int rc;

    //Remise du tableau a 0
    int i = 0;
    while (table_people[i].exist == 1)
    {
        table_people[i].exist = 0;
        i++;
    }
     Open database
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

     Create SQL statement
    sql = "SELECT * from FACIFICATOR";

     Execute SQL statement
    rc = sqlite3_exec(db, sql, callback_people, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return table_people;
}*/

int destroy_db()
{
    char *zErrMsg = 0;
    char *sql;
    sqlite3 *db = NULL;
    int rc;


    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "DELETE FROM FACIFICATOR";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Successfull destroy...\n\n");
    }
    sqlite3_close(db);

    //Suppresion de toutes les images dans le dossier picture
    DIR *directory;
    struct dirent *entry;
    struct stat file_stat;
    char buffer[1024] = {0};

    // On ouvre le dossier.
    directory = opendir("Pictures/");
    if ( directory == NULL )
    {
        fprintf(stderr, "cannot open directory %s\n", "Pictures/");
        return 0;
    }

    // On boucle sur les entrées du dossier.
    while ( (entry = readdir(directory)) != NULL ) {

        // On "saute" les répertoires "." et "..".
        if ( strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0 ) {
            continue;
        }

        snprintf(buffer, 1024, "%s/%s", "Pictures/", entry->d_name);
        stat(buffer, &file_stat);

        if ( S_ISREG(file_stat.st_mode) ) {
            // C'est un fichier. On le supprime.
            remove(buffer);
        }
    }

    // On ferme le dossier.
    closedir(directory);
    return 0;
}


int delete_id_db(char *id)
{
    //TODO : supprimer l'image correspondante dans le dossier Pictures
    sqlite3 *db = NULL;
    int rc;
    char *zErrMsg = 0;
    char *sql = NULL;

    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */

    sql = "DELETE FROM FACIFICATOR WHERE ID = ";
    char *result = malloc(strlen(sql) + strlen(id) + 1);
    strcpy(result, sql);
    strcat(result, id);
    sql = result;

    char *tmp = ";";
    result = malloc(strlen(sql) + strlen(tmp) + 1);
    strcpy(result, sql);
    strcat(result, tmp);
    sql = result;
    printf("%s\n", sql);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Successfull destroy...\n\n");
    }
    sqlite3_close(db);

    return 0;
}

people return_people_by_id(int id) {
    sqlite3 *db = NULL;
    int rc;
    char *zErrMsg = 0;
    char *sql = NULL;
    char *base_sql = NULL;

    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */

    base_sql = "SELECT NAME, PICTURE FROM FACIFICATOR WHERE ID = %d;";
    sql = malloc(sizeof(char) * 100);
    sprintf(sql, base_sql, id);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Successfull find...\n\n");
    }
    sqlite3_close(db);

    return tmp;
}

int insert(char *name, char *image)
{
    create_db();
    char *sql = NULL;
    sqlite3 *db = NULL;
    int rc;
    char *zErrMsg = 0;

    /* Open database*/
    rc = sqlite3_open("DataBase_facificator.db", &db);
    if ( rc )
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "INSERT INTO FACIFICATOR (NAME, PICTURE) VALUES ('";
    char *result = malloc(strlen(sql) + strlen(name) + 1);
    strcpy(result, sql);
    strcat(result, name);
    sql = result;

    char *chaine_tmp = "', '";
    result = malloc(strlen(sql) + strlen(chaine_tmp) + 1);
    strcpy(result, sql);
    strcat(result, chaine_tmp);
    sql = result;

    result = malloc(strlen(sql) + strlen(image) + 1);
    strcpy(result, sql);
    strcat(result, image);
    sql = result;


    chaine_tmp = "');";
    result = malloc(strlen(sql) + strlen(chaine_tmp) + 1);
    strcpy(result, sql);
    strcat(result, chaine_tmp);
    sql = result;
    printf("%s\n", sql);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        fprintf(stdout, "Successfull insert\n\n");
    }
    sqlite3_close(db);

    return 0;
}

//Cette fonction prend en paramètre ABSOLUTE path of the picture (*picture) and return the name or the new name if the file already exist
char* cp_picture_folder(char *picture)  //picture is ABSOLUTE path of the picture
{
    char src[FILENAME_MAX]; // FILENAME_MAX is in stdio.h
    FILE * fsrc;
    char *dest[FILENAME_MAX];
    *dest = NULL;

    fsrc = fopen(picture, "rb");
    if (fsrc == NULL)
        perror(src);
    else
    {
        FILE * fdest;

        char *filename = basename(picture);
        *dest = (char*) "Pictures/";

        //Ce paragraphe test si le dossier picture dans lequel on veut ecrire existe
        DIR* directory = opendir(*dest);
        FILE * file = NULL;

        //If the directory doesn't exist so we create
        if (directory == NULL)
        {
            do
            {
                file = fopen(*dest, "r");
                if (file == NULL)
                {
                    printf("The folder Pictures doesn't existe. Creating ......");
                    mkdir("Pictures", 0777);
                    printf("Done.\n");
                }
            }
            while (file == NULL);
            fclose(file);
        }

        char *result = malloc(strlen(*dest) + strlen(filename) + 1);
        strcpy(result, *dest);
        strcat(result, filename);
        *dest = result;

        if (strcmp(src, *dest) == 0)
            printf("The source can't also be the destination.\n");
        else
        {


            FILE * fdest2 = fopen(*dest, "r");
            //Si le fichier existe déjà on prépare les variables pour un renomage
            if (fdest2 != NULL)
            {
                int i = 1;
                //i<100 est une sécurité
                while (fopen(result, "r") != NULL && i < 100) {
                    printf("The file already exist .... Try an other name .....");

                    //Conversion de l'int i en char b
                    char b[100];
                    sprintf (b, "%d", i);

                    //Concaténation de l'underscore à b (anciennement i)
                    char *tmp1 = "_";
                    char *tmp = malloc(strlen(b) + strlen(tmp1) + 1);
                    strcpy(tmp, tmp1);
                    strcat(tmp, b);

                    //Concaténation de underscore+b au chemin
                    char *pDot = strrchr(*dest, '.');
                    char *tmp2 = '\0';

                    if (pDot != NULL)
                    {
                        tmp2 = pDot;
                        *(pDot + sizeof(tmp)) = '\0';
                    }

                    result = malloc(strlen(*dest) - strlen(pDot) + strlen(tmp) + 1);
                    strcpy(result, *(dest));
                    strcpy(result + strlen(*dest) - strlen(pDot), tmp);

                    char* result2 = malloc(strlen(result) + strlen(tmp2) + 1);
                    strcpy(result2, result);
                    strcat(result2, tmp2);
                    result = result2;

                    i++;
                }
                *dest = result;

            }
            fdest = fopen(*dest, "wb");

            if (fdest == NULL)
            {
                printf("Fail cp ...\n");
                perror(*dest);
            }
            printf("%s is a correct name\n", *dest);
            int c;

            while ((c = getc(fsrc)) != EOF)
                putc(c, fdest);

            fclose(fdest);
            printf("Copy done.\n");

        }
        fclose(fsrc);
    }
    return *dest ;
}

//Insertion en masse de visages où le nom est le nom du fichier
int insert_folder (char *folder)
{
    create_db();
    printf("Launch  processing of all files\n");

    DIR *rep = opendir (folder);
    struct dirent *dir = NULL;

    if (!rep)
    {
        fprintf (stderr, "error: opendir failed on '%s'\n", folder);
        return 0;
    }

    while ((dir = readdir(rep)))
    {
        if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
        {
            // do nothing (straight logic)

            ////Si le "fichier" actuel que l'on traite et le . du fichier courant on zap ou encore si c'est le rep parent
        }
        else {
            char buf[_POSIX_PATH_MAX] = {0};
            char entry[_POSIX_PATH_MAX] = {0};
            char *path = NULL;
            errno = 0;
            strcat (entry, folder);
            strcat (entry, "/");
            strcat (entry, dir->d_name);
            printf ("getting realpath for : '%s'\n", entry);
            path = realpath (entry, buf);
            if (!path || errno)
            {
                perror("realpath");
                exit(EXIT_FAILURE);
            }

            printf (" realpath for '%s' : %s\n", entry, buf);

            printf("Open : %s \n", path);

            char* name_after_copy = cp_picture_folder(buf);

            if (name_after_copy == NULL)
            {
                printf("The copy function return null... Stop .... \n");
                return 0;
            }

            //Suppresion de l'extension du fichier pour le nom de la personne

            char *pDot = strrchr(dir->d_name, '.');
            if (pDot != NULL)
                *pDot = '\0';

            insert(dir->d_name, name_after_copy);
        }
    }

    closedir(rep);
    return (0);
}



/*int main()
{
    create_db();
    getchar();
      insert("'name of person 1'","'path of picture'");
      getchar();
      insert("'name of person 2'","'path of picture'");
      getchar();
      delete_id_db("2");
      getchar();
      see_all_db();
      getchar();
      destroy_db();
      getchar();
      create_db();
      getchar();
      cp_picture_folder("test/test.png");
      getchar();
    insert_folder("test/");
    see_all_db();
    destroy_db();
    return 0;
}
*/
