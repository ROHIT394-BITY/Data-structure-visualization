#include "common.h"      
#include "linked_list.c"
#include "stack.c"
#include "queue.c"
#include "avl.c"
#include "hash.c"

void update_status(GtkWidget *label, const char *msg, GtkWidget *area) {
    char styled_msg[512];
    if (strstr(msg, "Error") || strstr(msg, "Invalid") || strstr(msg, "not found") || strstr(msg, "Empty") || strstr(msg, "Out of Bounds")) {
        sprintf(styled_msg, "<span font='16' weight='bold' foreground='#c0392b'>%s</span>", msg); 
    } else if (strstr(msg, "Found") || strstr(msg, "Inserted") || strstr(msg, "Pushed") || strstr(msg, "Enqueued") || strstr(msg, "Deleted") || strstr(msg, "Popped") || strstr(msg, "Dequeued")) {
        sprintf(styled_msg, "<span font='16' weight='bold' foreground='#27ae60'>%s</span>", msg); 
    } else {
        sprintf(styled_msg, "<span font='16' weight='bold' foreground='#7f8c8d'>%s</span>", msg);
    }
    gtk_label_set_markup(GTK_LABEL(label), styled_msg);
    if (area) gtk_widget_queue_draw(area);
}

int get_integer_input(GtkWidget *entry) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (strlen(text) == 0) return -999999;
    char *end;
    long val = strtol(text, &end, 10);
    if (end == text || *end != '\0') return -999999;
    return (int)val;
}

void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background-color: #f4f6f7; }"
        
        "stackswitcher { "
        "   background-color: #2c3e50; " 
        "   padding: 0; "
        "   border-bottom: 3px solid #34495e; "
        "}"
        
        "stackswitcher button { "
        "   color: #ecf0f1; " 
        "   background-color: #2c3e50; "
        "   margin: 0; "
        "   padding: 12px 30px; " 
        "   border: none; "
        "   border-radius: 0; "
        "   font-size: 16px; "
        "   font-weight: 600; "
        "   outline: none; "
        "   transition: background-color 0.2s; "
        "}"
        
        "stackswitcher button:hover { "
        "   background-color: #34495e; " 
        "   color: #ffffff; "
        "}"
        
        "stackswitcher button:checked { "
        "   color: #ffffff; " 
        "   background-color: #3498db; " 
        "   border-bottom: 4px solid #e74c3c; " 
        "}"
        
        "stackswitcher button label { "
        "   color: inherit; "
        "   font-weight: inherit; "
        "}"
        
        "box { "
        "   background-color: white; "
        "   border-radius: 8px; "
        "   margin: 15px; "
        "   padding: 15px; "
        "   box-shadow: 0 2px 8px rgba(0,0,0,0.05); "
        "}" 
        

        "button { "
        "   padding: 8px 18px; "
        "   background-image: none; "
        "   background-color: #3498db; "
        "   color: white; "
        "   border-radius: 5px; "
        "   border: none; "
        "   margin: 0 4px; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}"
        "button:hover { background-color: #2980b9; }"
        "button:active { background-color: #16a085; }"
        
    
        "entry { "
        "   padding: 8px; "
        "   border-radius: 5px; "
        "   border: 2px solid #bdc3c7; "
        "   background-color: #ffffff; "
        "   color: #2c3e50; "
        "}"
        "entry:focus { border-color: #3498db; }"
        

        "label { "
        "   color: #34495e; "
        "   font-weight: bold; "
        "   font-size: 14px; "
        "}",
        -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static void activate(GtkApplication *app, gpointer user_data) {
    apply_css();
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Interactive Data Structure Visualizer - DataStellar");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *stack = gtk_stack_new();
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));
    gtk_box_pack_start(GTK_BOX(vbox), switcher, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), stack, TRUE, TRUE, 0);

    gtk_stack_add_titled(GTK_STACK(stack), create_ll_page(), "ll", "Linked List");
    gtk_stack_add_titled(GTK_STACK(stack), create_stack_page(), "stack", "Stack");
    gtk_stack_add_titled(GTK_STACK(stack), create_queue_page(), "queue", "Queue");
    gtk_stack_add_titled(GTK_STACK(stack), create_avl_page(), "avl", "AVL Tree");
    gtk_stack_add_titled(GTK_STACK(stack), create_hash_page(), "hash", "Hash Table");

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.datastellar.visualizer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
