// TODO: Add a list of text boxes, our 'hoarded flags'
// Also call the flag checker function
// Maybe figure out how to not highlight the serial text box, since it covers up the rarctf{} prompt
#include <stdbool.h>
#include <gtk/gtk.h>

static unsigned int remaining = 40 * 24 * 60 * 60;
GtkWidget* countdown;
int do_crc_check(char* buf);
int do_xor_check(char* buf);

bool registered = false;

gint step_timer(gpointer data) {
    if (!registered) {
        remaining--;
        if (remaining == 0)
            exit(-1);
        char buf[30];
        unsigned int n = remaining;
        int day = n/(24*3600);
        n = n % (24*3600);
        int hour = n / 3600;
        n %= 3600;
        int minutes = n / 60;
        n %= 60;
        int seconds = n;
        sprintf(buf, "%d:%d:%d:%d", day, hour, minutes, seconds);
        gtk_label_set_text(GTK_LABEL(countdown), buf);
    } else {
        gtk_label_set_text(GTK_LABEL(countdown), "Application Registered");
    }
    return 1;
}

void func(GtkDialog* dialog, int respid, gpointer data) {
    GtkEntry* entry = GTK_ENTRY(data);
    char buffer[0x64] = {0};
    switch (respid) {
    case GTK_RESPONSE_ACCEPT:
        memcpy(buffer, gtk_entry_get_text(entry), 0x64);
        if (!do_crc_check(buffer)) {
            puts("Invalid Serial Key 1");
        } else if (!do_xor_check(buffer)) {
            puts("Invalid Serial Key 2");
        } else {
            registered = true;
        }
    case GTK_RESPONSE_REJECT:
        gtk_widget_destroy(GTK_WIDGET(dialog));
    case GTK_RESPONSE_DELETE_EVENT:
        break;
    }
}

void prompt_serial(GtkWindow* parent, gpointer data) {
    GtkWidget* dialog, *label, *content, *entry;
    GtkDialogFlags flags;

    label = gtk_label_new("Serial key: ");
    entry = gtk_entry_new();
    gtk_entry_set_icon_tooltip_text(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "rarctf{...}");
    gtk_entry_set_activates_default(GTK_ENTRY(entry), FALSE);
    flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
    dialog = gtk_dialog_new_with_buttons("Register",
                                         parent,
                                         flags,
                                         "_Cancel",
                                         GTK_RESPONSE_REJECT,
                                         "_Submit",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

    // g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(func),
                             entry);
    gtk_container_add(GTK_CONTAINER(content), label);
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);
}

static void activate(GtkApplication* app, gpointer data) {
    GtkWidget* window, *button, *text, *grid, *layout, *image;

    countdown = gtk_label_new("40:00:00:00");
    window = gtk_application_window_new(app);
    text = gtk_label_new("Welcome to Flag Hoarder. Time Remaining in Trial:");
    button = gtk_button_new();
    g_signal_connect(button, "clicked", G_CALLBACK(prompt_serial), NULL);
    grid = gtk_grid_new();
    gtk_window_set_title(GTK_WINDOW(window), "IFT");
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 300);
    gtk_container_add(GTK_CONTAINER(window), grid);

    image = gtk_image_new_from_file("./rars.png");
    gtk_grid_attach(GTK_GRID(grid), image, 0, 2, 2, 2);


    gtk_widget_set_hexpand(text, TRUE);
    gtk_widget_set_hexpand(button, TRUE);
    gtk_button_set_label(GTK_BUTTON(button), "Click To Register Product");
    gtk_grid_attach(GTK_GRID(grid), text, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), countdown, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);
    g_timeout_add(1000, step_timer, NULL);
    gtk_widget_show_all(window);
    // prompt_serial(GTK_WINDOW(window));
}

int main(int argc, char** argv) {
    GtkApplication* app;
    int status;
    app = gtk_application_new("win.rars.ift", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
