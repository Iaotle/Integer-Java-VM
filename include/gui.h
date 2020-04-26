#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h> /* GTK-3.0 library */


/**
 * This gets called straight out of main, and quits only when you press "Quit" or on SIGINT
 */
void rungui(void);

/**
 * This creates the GUI window and everything in it
 */
void activate(GtkApplication *app, gpointer user_data);

/**
 * Function for the select ijvm file button
 */
void selectfile(GtkWidget *window);

/**
 * Function for the select output file button
 */
void selectoutput(GtkWidget *widget, gpointer data);

/**
 * Function for the select input file button
 */
void selectinput(GtkWidget *widget, gpointer data);

/**
 * Function for the start IJVM button
 */
void start(void);


#endif