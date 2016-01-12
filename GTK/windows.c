

# include <gtk/gtk.h>
# include <stdlib.h>
# include <stdio.h>
# include "windows.h"
# include "../Database/BDD.h"
#include "../SDL/predict.h"

typedef struct {
  GtkBuilder *builder;
  gpointer user_data;
}

SGlobalData;

/*VARIABLE*/
GtkWidget *fenetre_ajout = NULL;
GtkWidget *fenetre_ajout_basededonnee = NULL;
GtkFileChooser *file_selector = NULL;
GtkBuilder *builder = NULL;
GtkWidget *bouton_close_window_load_image = NULL;
GtkWidget *bouton_ok_window_load_image = NULL;
gchar *filepath = NULL;
GtkImage *image = NULL;
GtkWidget *buton_detection = NULL;
GtkWidget *button_ajout = NULL;
GtkWidget *buton_ok_fenetre_ajout_basededonnee = NULL;
GtkWidget *buton_annuler_fenetre_ajout_basededonnee = NULL;
gchar *name = NULL;
GtkEntry *champ_name = NULL;
GtkFileChooser *file_selector2 = NULL;
GtkWidget *table_db = NULL;
GtkWidget *fenetre_table_basededonnee = NULL;
GtkWidget *buton_affichage_db = NULL;
GtkWidget *fenetre_suppression_by_id_db = NULL;
GtkWidget *buton_ok_suppression_db = NULL;
GtkSpinButton *spin_button_suppression_id = NULL;
GtkWidget *buton_suppression_by_id_db = NULL;





GtkWidget *label = NULL;





void callback_about (GtkMenuItem *menuitem, gpointer user_data);

void on_Ajout_clicked();

int main(int argc, char *argv []) {
  GtkWidget *fenetre_principale = NULL;
  //GtkWidget *chargement_image = NULL;
  //GtkWidget *bouton_ajout = NULL;
  //GtkBuilder *builder1 = NULL;
  GError *error = NULL;
  gchar *filename = NULL;


  /* Initialisation de la librairie Gtk. */
  gtk_init(&argc, &argv);


  /* Ouverture du fichier Glade de la fenêtre principale */
  builder = gtk_builder_new();

  /* Création du chemin complet pour accéder au fichier test.glade. */
  /* g_build_filename(); construit le chemin complet en fonction du système */
  /* d'exploitation. ( / pour Linux et \ pour Windows) */
  filename =  g_build_filename ("Windows.glade", NULL);

  /* Chargement du fichier test.glade. */
  gtk_builder_add_from_file (builder, filename, &error);
  g_free (filename);
  if (error) {
    gint code = error->code;
    g_printerr("%s\n", error->message);
    g_error_free (error);
    return code;
  }

  /* Récupération du pointeur de la fenêtre principale */
  fenetre_principale = GTK_WIDGET(gtk_builder_get_object (builder, "window1"));
  image = GTK_IMAGE(gtk_builder_get_object (builder, "image1"));
  buton_detection = GTK_WIDGET(gtk_builder_get_object (builder, "button2"));
  g_signal_connect(G_OBJECT(buton_detection), "clicked", G_CALLBACK(on_button_detection), G_OBJECT (fenetre_ajout));

  button_ajout = GTK_WIDGET(gtk_builder_get_object (builder, "button4"));
  g_signal_connect(G_OBJECT(button_ajout), "clicked", G_CALLBACK(on_ajout_button), G_OBJECT (fenetre_ajout));

// buton_affichage_db =  GTK_WIDGET(gtk_builder_get_object (builder, "button6"));
// g_signal_connect(G_OBJECT(buton_affichage_db), "clicked", G_CALLBACK(fenetre_affichage_db), G_OBJECT (fenetre_ajout));

  buton_suppression_by_id_db = GTK_WIDGET(gtk_builder_get_object(builder, "button5"));
  g_signal_connect(G_OBJECT(buton_suppression_by_id_db), "clicked", G_CALLBACK(open_fenetre_suppression_by_id_db), G_OBJECT (fenetre_ajout));





  /* Affectation du signal "destroy" à la fonction gtk_main_quit(); pour la */
  /* fermeture de la fenêtre. */
  g_signal_connect(G_OBJECT(fenetre_principale), "destroy", (GCallback)gtk_main_quit, NULL);
  //g_signal_connect(G_OBJECT(chargement_image), "load_image", G_CALLBACK(on_load_clicked), NULL);*/

  /*FENETRE CHARGEMENT IMAGE*/
  fenetre_ajout = GTK_WIDGET(gtk_builder_get_object (builder, "applicationwindow1"));

  bouton_close_window_load_image =  GTK_WIDGET (gtk_builder_get_object (builder, "button8"));
  bouton_ok_window_load_image = GTK_WIDGET (gtk_builder_get_object (builder, "button7"));

  //Le premier faisait un core dumped quand on cliquer sur annuler et on re-ouvrer la fenetre
  //g_signal_connect(gtk_builder_get_object(builder,"button1"),"clicked",G_CALLBACK(on_load_clicked),(builder)),gtk_entry_get_text (GTK_ENTRY (gtk_builder_get_object (builder, "entry1")));
  g_signal_connect(gtk_builder_get_object(builder, "button1"), "clicked",
                   G_CALLBACK(on_load_clicked), (builder));
  //Bouton OK -->appelle la fct de traitements
  g_signal_connect(G_OBJECT(bouton_ok_window_load_image), "clicked",
                   G_CALLBACK(valid_choose_picture), G_OBJECT (fenetre_ajout));
  //Bouton OK -->ferme la boite de dialogue (execution après l'autre fct)
  //g_signal_connect(G_OBJECT(bouton_ok_window_load_image), "clicked",
  //                G_CALLBACK(close_window), G_OBJECT (fenetre_ajout));
  //Bouton annuler --> ferme la boite de dialogue
  g_signal_connect(G_OBJECT(bouton_close_window_load_image), "clicked",
                   G_CALLBACK(close_window_ajout), G_OBJECT (fenetre_ajout));
  //Fileselector
  file_selector = GTK_FILE_CHOOSER(gtk_builder_get_object (builder, "filechooserbutton1"));
  g_signal_connect(G_OBJECT(file_selector), "selection-changed",
                   G_CALLBACK(fileselector_selection_changed), G_OBJECT (fenetre_ajout));

  /*FENETRE SUPPRESSION DANS LA BASE DE DONNEE*/
  fenetre_suppression_by_id_db = GTK_WIDGET(gtk_builder_get_object(builder, "applicationwindow4"));
  spin_button_suppression_id = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton1"));
  buton_ok_suppression_db = GTK_WIDGET(gtk_builder_get_object(builder, "button11"));
  g_signal_connect(G_OBJECT(buton_ok_suppression_db), "clicked", G_CALLBACK(button_ok_suppression), G_OBJECT(fenetre_suppression_by_id_db));



  /*FENETRE AJOUT BASE DE DONNEE*/
  fenetre_ajout_basededonnee =
    GTK_WIDGET(gtk_builder_get_object(builder, "applicationwindow2"));

  buton_ok_fenetre_ajout_basededonnee =  GTK_WIDGET (gtk_builder_get_object
                                         (builder, "button10"));

  buton_annuler_fenetre_ajout_basededonnee
    =  GTK_WIDGET (gtk_builder_get_object (builder, "button9"));

  g_signal_connect(G_OBJECT(buton_ok_fenetre_ajout_basededonnee), "clicked",
                   G_CALLBACK(ajout_people_in_db), G_OBJECT (fenetre_ajout_basededonnee));
  //g_signal_connect(G_OBJECT(buton_ok_fenetre_ajout_basededonnee),"clicked",G_CALLBACK(close_window),G_OBJECT (fenetre_ajout_basededonnee));

  g_signal_connect(G_OBJECT(buton_annuler_fenetre_ajout_basededonnee),
                   "clicked", G_CALLBACK(close_window_ajout_in_db), G_OBJECT (fenetre_ajout_basededonnee));

  file_selector2 = GTK_FILE_CHOOSER(gtk_builder_get_object (builder,
                                    "filechooserbutton2"));

  g_signal_connect(G_OBJECT(file_selector2),
                   "selection-changed", G_CALLBACK(fileselector2_selection_changed),
                   G_OBJECT (fenetre_ajout_basededonnee));

  champ_name = GTK_ENTRY(gtk_builder_get_object(builder, "entry1"));

  g_signal_connect(G_OBJECT(champ_name), "changed",
                   G_CALLBACK(champ_name_changed), G_OBJECT (fenetre_ajout_basededonnee));


  /*FENETRE TABLEAU BASE DE DONNEE*/
  fenetre_table_basededonnee = GTK_WIDGET(gtk_builder_get_object (builder, "applicationwindow3"));
  table_db = GTK_WIDGET (gtk_builder_get_object
                         (builder, "grid2"));



  /* Affichage de la fenêtre principale. */ gtk_widget_show_all
  (fenetre_principale);




  gtk_main();

  return 0;
}


void on_load_clicked() {

  //TODO : A faire quand j'aurais la foi
  /*GtkFileFilter *filtre
  = gtk_file_filter_new (); filtre.set_name("All files");
  filtre.add_pattern("*"); file_selector.add_filter(filtre);

  filtre = gtk_file_filter_new (); filtre = gtk.FileFilter();
  filtre.set_name("Images"); filtre.add_mime_type("image/png");
  filtre.add_mime_type("image/jpeg"); filtre.add_mime_type("image/gif");
  filtre.add_pattern("*.png"); filtre.add_pattern("*.jpg");
  filtre.add_pattern("*.gif"); filtre.add_pattern("*.tif");
  filtre.add_pattern("*.xpm"); filtre.add_pattern("*.bmp");
  file_selector.add_filter(filtre);*/

  /* char* filename; filename = file_selector->get_filename();*/


  gtk_widget_show_all (fenetre_ajout);
}

void fileselector_selection_changed() {
  filepath = NULL;
  filepath = gtk_file_chooser_get_filename(file_selector);
}

void valid_choose_picture() {
  if (filepath == NULL) {
    printf("Error filepath is NULL\n"); return;
  } printf("Fichier sélectionner : %s\n", filepath);
  gtk_image_set_from_file(image, filepath);
  gtk_widget_hide(fenetre_ajout);

}

void close_window( gpointer window) {
  //gtk_widget_destroy(GTK_WIDGET(window));
  gtk_widget_hide(window);
}

void close_window_ajout() {
  gtk_widget_hide(fenetre_ajout);

}

void close_window_ajout_in_db() {
  gtk_widget_hide(fenetre_ajout_basededonnee);
}


/*void open_window( gpointer window) {
  //gtk_widget_destroy(GTK_WIDGET(window));
  //gtk_widget_hide_all(window);
}*/

void on_button_detection() {
  printf("Yannick, à toi !\n");
  if (filepath != NULL) {
    printf("Chemin pour yannick%s\n", filepath );
    SDL_Surface *surface = load_image(filepath);
    scan_image(surface);
    gtk_image_set_from_file(image, "../SDL/detection.bmp");
  }
  else  {
    printf("Chemin nul ...\n");
  }

}

void on_ajout_button() {
  //name = NULL; filepath = NULL;
  gtk_widget_show_all(fenetre_ajout_basededonnee);

}

void ajout_people_in_db() {
  if (name == NULL || filepath == NULL) {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT; GtkWidget *dialog =
      gtk_message_dialog_new (fenetre_ajout,
                              flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                              "Veuillez remplir les champs correctement");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
  } else {
    insert(name, filepath);
    //gtk_widget_destroy (fenetre_ajout_basededonnee);
    gtk_widget_hide(fenetre_ajout_basededonnee);

  }

  /*GtkWidget *messagedialog = gtk.MessageDialog(parent=None, flags=0,
  type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_CLOSE, message_format="Veuillez
  remplir les champs correctement")
   result = messagedialog.run() //Lance la boîte de dialogue
   messagedialog.destroy() //Détruit la boîte de dialogue printf("An
   exception rised!\n");*/


}

void fileselector2_selection_changed() {
  filepath = NULL;
  filepath = gtk_file_chooser_get_filename(file_selector2);
}

void champ_name_changed() {
  name =  gtk_entry_get_text(GTK_ENTRY(champ_name));
}

void fenetre_affichage_db() {
  //GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(fenetre_affichage_db));
  gtk_widget_show_all(fenetre_affichage_db);
}

void open_fenetre_suppression_by_id_db() {
  GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(fenetre_suppression_by_id_db));
  gtk_widget_show_all(win);
}

void button_ok_suppression() {
  int id = gtk_spin_button_get_value_as_int(spin_button_suppression_id);
  printf("L'id est : %d\n", id );
  // people this = return_people_by_id(id);
  // char *b = malloc(sizeof(char )* 100);
  // sprintf(b,"%d",id);
  // GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  // GtkWidget *dialog =
  //   gtk_message_dialog_new (fenetre_ajout,
  //                           flags, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
  //                           "Etes vous sûr de vouloir supprimer %s", this.name);
  // switch (gtk_dialog_run (GTK_DIALOG (dialog)))
  // {
  // case GTK_RESPONSE_OK:
  //   delete_id_db(b);
  //   break;
  // }
  char b[50];
  sprintf(b, "%d", id);
  delete_id_db(b);

// gtk_widget_destroy (dialog);
  GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(fenetre_suppression_by_id_db));
  gtk_widget_hide(win);

}