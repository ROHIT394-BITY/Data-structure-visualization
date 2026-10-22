#include "common.h"

static struct Node* stack_top = NULL;

static gboolean on_draw_stack(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int w = gtk_widget_get_allocated_width(widget);
    int h_canvas = gtk_widget_get_allocated_height(widget);

    cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, h_canvas);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.95, 0.95, 0.95);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.9, 0.9, 0.9);
    cairo_set_source(cr, pat);
    cairo_paint(cr);
    cairo_pattern_destroy(pat);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);

    int x = w/2 - 50, y = 50;
    int nw = 100, nh = 40;
    char buf[32];
    cairo_text_extents_t ext;

    cairo_text_extents(cr, "TOP", &ext);
    cairo_move_to(cr, x - ext.width - 10, y + nh/2);
    cairo_show_text(cr, "TOP");
    cairo_move_to(cr, x - 5, y + nh/2);
    cairo_line_to(cr, x, y + nh/2);
    cairo_stroke(cr);

    struct Node *t = stack_top;
    if(!t) {
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        cairo_rectangle(cr, x, y, nw, nh);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
        cairo_rectangle(cr, x, y, nw, nh);
        cairo_stroke(cr);
        
        cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
        cairo_text_extents(cr, "NULL", &ext);
        cairo_move_to(cr, x + (nw-ext.width)/2, y + (nh+ext.height)/2);
        cairo_show_text(cr, "NULL");
    }

    while(t) {
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_rectangle(cr, x, y, nw, nh);
        cairo_fill(cr);
        
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x, y, nw, nh);
        cairo_stroke(cr);
        
        sprintf(buf, "%d", t->data);
        cairo_text_extents(cr, buf, &ext);
        cairo_move_to(cr, x + (nw-ext.width)/2, y + (nh+ext.height)/2);
        cairo_show_text(cr, buf);

        cairo_move_to(cr, x + nw/2, y + nh);
        cairo_line_to(cr, x + nw/2, y + nh + 20);
        cairo_stroke(cr);
        
        y += nh + 20;
        t = t->next;

        if (!t) {
            cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
            cairo_rectangle(cr, x, y, nw, nh);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
            cairo_rectangle(cr, x, y, nw, nh);
            cairo_stroke(cr);
            
            cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
            cairo_text_extents(cr, "NULL", &ext);
            cairo_move_to(cr, x + (nw-ext.width)/2, y + (nh+ext.height)/2);
            cairo_show_text(cr, "NULL");
        }
    }
    return FALSE;
}

static void stack_act(GtkButton *b, gpointer data) {
    struct CommonControls *c = (struct CommonControls*)data;
    const char *lbl = gtk_button_get_label(b);
    int val = get_integer_input(c->value_entry);
    char msg[64];

    if (strstr(lbl, "Push")) {
        if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }
        struct Node *n = malloc(sizeof(struct Node));
        n->data = val; n->next = stack_top; stack_top = n;
        sprintf(msg, "Pushed %d", val);
        gtk_entry_set_text(GTK_ENTRY(c->value_entry), "");
    } else if (strstr(lbl, "Pop")) {
        if (!stack_top) { update_status(c->status_label, "Stack Empty", NULL); return; }
        struct Node *t = stack_top;
        int d = t->data;
        stack_top = stack_top->next;
        free(t);
        sprintf(msg, "Popped %d", d);
    } else {
        if (!stack_top) { update_status(c->status_label, "Stack Empty", NULL); return; }
        sprintf(msg, "Top: %d", stack_top->data);
    }
    update_status(c->status_label, msg, c->drawing_area);
}

GtkWidget* create_stack_page() {
    GtkWidget *grid = gtk_grid_new();
    struct CommonControls *c = g_malloc(sizeof(struct CommonControls));
    
    c->drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(c->drawing_area, 800, 450);
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scroll, TRUE); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), c->drawing_area);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 0, 1, 1);

    GtkWidget *ctrl_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(ctrl_box, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(ctrl_box, 10);
    gtk_widget_set_margin_bottom(ctrl_box, 10);
    gtk_grid_attach(GTK_GRID(grid), ctrl_box, 0, 1, 1, 1);

    c->value_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(c->value_entry), "Value");
    gtk_box_pack_start(GTK_BOX(ctrl_box), c->value_entry, FALSE, FALSE, 0);

    const char* btns[] = {"Push", "Pop", "Peek"};
    for(int i=0; i<3; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i]);
        g_signal_connect(b, "clicked", G_CALLBACK(stack_act), c);
        gtk_box_pack_start(GTK_BOX(ctrl_box), b, FALSE, FALSE, 0);
    }
    g_signal_connect(c->drawing_area, "draw", G_CALLBACK(on_draw_stack), NULL);

    c->status_label = gtk_label_new("Ready");
    gtk_label_set_use_markup(GTK_LABEL(c->status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), c->status_label, 0, 2, 1, 1);
    g_object_add_weak_pointer(G_OBJECT(grid), (gpointer *)&c);
    return grid;
}
