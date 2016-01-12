 #include <gtk/gtk.h>
# include <stdlib.h>
# include <stdio.h>

typedef struct people people;
struct people
{
    int exist;
    int id;
    char* name;
    char* picture;
} ;

void on_load_clicked();
void close_window( gpointer window);
void valid_choose_picture();
void fileselector_selection_changed();
void on_button_detection();
void on_ajout_button();
void fileselector2_selection_changed();
void ajout_people_in_db();
void champ_name_changed();
void fenetre_affichage_db();
void button_ok_suppression();
void open_fenetre_suppression_by_id_db();
void close_window_ajout();
void close_window_ajout_in_db();