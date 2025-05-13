#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

char text[100][200];
int row = 0;
int column = 0;

gboolean cursor_visible = TRUE;

GtkWidget *fixed;

void update_labels() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(fixed));
    for (GList *child = children; child != NULL; child = child->next)
        gtk_widget_destroy(GTK_WIDGET(child->data));
    g_list_free(children);

    for (int i = 0; i <= row; i++) {
        char buffer[201];
        if (i == row && cursor_visible) {
            strncpy(buffer, text[i], column);
            buffer[column] = '|';
            strcpy(buffer + column + 1, text[i] + column);
        } else {
            strcpy(buffer, text[i]);
        }

        GtkWidget *label = gtk_label_new(buffer);
        gtk_fixed_put(GTK_FIXED(fixed), label, 10, i * 20);
    }

    gtk_widget_show_all(fixed);
}

char* prompt_filename(GtkWindow *parent, const char *title) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        title,
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_OK", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *label = gtk_label_new(NULL);
    if (g_strcmp0(title, "Save As") == 0)
        gtk_label_set_text(GTK_LABEL(label), "Save file\nEnter file name with extension:");
    else
        gtk_label_set_text(GTK_LABEL(label), "Open file\nEnter file name with extension:");

    GtkWidget *entry = gtk_entry_new();

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(content), vbox);

    gtk_widget_show_all(dialog);

    char *filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *input = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(input) > 0) {
            filename = g_strdup(input); 
        }
    }

    gtk_widget_destroy(dialog);
    return filename;
}

gboolean onkeypress(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    gunichar ch = gdk_keyval_to_unicode(event->keyval);
    gboolean ctrl = (event->state & GDK_CONTROL_MASK);

    if (ctrl && event->keyval == GDK_KEY_s) {
        char *filename = prompt_filename(GTK_WINDOW(widget), "Save As");
        if (filename) {
            FILE *f = fopen(filename, "w");
            if (f) {
                for (int i = 0; i <= row; i++)
                    fprintf(f, "%s\n", text[i]);
                fclose(f);
                g_print("Saved to %s\n", filename);
            } else {
                g_print("Failed to open file for writing: %s\n", filename);
            }
            g_free(filename);
        }
        return TRUE;
    }

    if (ctrl && event->keyval == GDK_KEY_o) {
        char *filename = prompt_filename(GTK_WINDOW(widget), "Open File");
        if (filename) {
            FILE *f = fopen(filename, "r");
            if (f) {
                row = 0;
                column = 0;
                memset(text, 0, sizeof(text));
                char line[200];
                while (fgets(line, sizeof(line), f) && row < 100) {
                    line[strcspn(line, "\n")] = '\0';
                    strncpy(text[row], line, sizeof(text[row]) - 1);
                    row++;
                }
                fclose(f);
                row--;
                column = strlen(text[row]);
                update_labels();
                g_print("Loaded from %s\n", filename);
            } else {
                g_print("Failed to open file for reading: %s\n", filename);
            }
            g_free(filename);
        }
        return TRUE;
    }

    if (event->keyval == GDK_KEY_Return) {
        row++;
        column = 0;
    } else if (event->keyval == GDK_KEY_space) {
        text[row][column++] = ' ';
        text[row][column] = '\0';
    } else if (event->keyval == GDK_KEY_BackSpace) {
        if (column > 0) {
            text[row][--column] = '\0';
        } else if (row > 0) {
            row--;
            column = strlen(text[row]);
        }
    } else if (event->keyval == GDK_KEY_Left) {
        if (column > 0) {
            column--;
        } else if (row > 0) {
            row--;
            column = strlen(text[row]);
        }
    } else if (event->keyval == GDK_KEY_Right) {
        if (column < strlen(text[row])) {
            column++;
        } else if (row < 99 && strlen(text[row + 1]) > 0) {
            row++;
            column = 0;
        }
    } else if (event->keyval == GDK_KEY_Up) {
        if (row > 0) {
            row--;
            if (column > strlen(text[row])) {
                column = strlen(text[row]);
            }
        }
    } else if (event->keyval == GDK_KEY_Down) {
        if (row < 99 && strlen(text[row + 1]) > 0) {
            row++;
            if (column > strlen(text[row])) {
                column = strlen(text[row]);
            }
        }
    } else if (g_unichar_isprint(ch)) {
        text[row][column++] = (char)ch;
        text[row][column] = '\0';
    }

    update_labels();
    return FALSE;
}

gboolean toggle_cursor(gpointer data) {
    cursor_visible = !cursor_visible;
    update_labels();
    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    g_timeout_add(500, (GSourceFunc)toggle_cursor, NULL);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Veem");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // Enable key input
    gtk_widget_set_can_focus(window, TRUE);
    gtk_widget_grab_focus(window);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

    update_labels();  // Initial display

    // Signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(onkeypress), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
