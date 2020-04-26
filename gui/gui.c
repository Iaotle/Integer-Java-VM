#include <stdio.h>
#include <gtk/gtk.h>
#include "ijvm.h"
#include "gui.h"


char *filename;
FILE *output;
FILE *input;
GtkWidget *buttonstart; //this is to make sure the greying out stuff works fine


void rungui(){
    GtkApplication *app;

    app = gtk_application_new ("iaotle.github.io", G_APPLICATION_FLAGS_NONE); // gtkapplication pointer app
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL); //connect
    g_application_run (G_APPLICATION (app), 0, 0);
    g_object_unref (app);

}

void activate (GtkApplication *app, gpointer user_data)
{
    GtkWidget *window,*contentbox, *buttonselect, *buttonoutput, *buttoninput, *buttonquit;
    GtkEntry *entry;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "IJVM");
    gtk_window_set_default_size (GTK_WINDOW (window), 650, 30);

    entry = (GtkEntry*)gtk_entry_new();
    gtk_entry_set_placeholder_text (entry, "I/O");

    buttoninput = gtk_button_new_with_label("Set input string");
    buttonoutput = gtk_button_new_with_label ("Set output file");
    buttonselect = gtk_button_new_with_label ("Select .ijvm file");
    buttonstart = gtk_button_new_with_label ("Run IJVM");
    buttonquit = gtk_button_new_with_label ("Quit");

    gtk_widget_set_sensitive(buttonstart, false); //can't start without loading binary

    g_signal_connect (buttoninput, "clicked", G_CALLBACK(selectinput), entry);
    g_signal_connect (buttonoutput, "clicked", G_CALLBACK (selectoutput), entry);
    g_signal_connect (buttonselect, "clicked", G_CALLBACK (selectfile), window);
    g_signal_connect (buttonstart, "clicked", G_CALLBACK (start), window);
    g_signal_connect_swapped (buttonquit, "clicked", G_CALLBACK (gtk_widget_destroy), window);

    contentbox = gtk_box_new(0,0);
    gtk_box_pack_start(GTK_BOX(contentbox), (GtkWidget*)entry, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(contentbox), buttoninput, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(contentbox), buttonoutput, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(contentbox), buttonselect, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(contentbox), buttonstart, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(contentbox), buttonquit, 0, 0, 0);
    gtk_container_add (GTK_CONTAINER (window), contentbox);

    gtk_widget_show_all (window);
}

void selectfile(GtkWidget *window){
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    dialog = gtk_file_chooser_dialog_new ("Open File", (GtkWindow*)window, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        gtk_widget_set_sensitive(buttonstart, true); //enable running the IJVM
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
    }
    gtk_widget_destroy (dialog);
}

void selectoutput(GtkWidget *widget, gpointer data){
    g_print("Output file set to %s\n", gtk_entry_get_text(GTK_ENTRY(data)));
    output = fopen(gtk_entry_get_text(GTK_ENTRY(data)), "w+");
    set_output(output);
}

void selectinput(GtkWidget *widget, gpointer data){
    g_print("Input set to %s\n", gtk_entry_get_text(GTK_ENTRY(data)));
    input = fopen("input.txt", "w+");
    fprintf(input, "%s", gtk_entry_get_text(GTK_ENTRY(data)));
    rewind(input);
    set_input(input);
}

void start(){
    init_ijvm(filename);

    run();

    destroy_ijvm();

    if(input){
        rewind(input); //no need to reenter input string every time
    }
}