#include "common.h"

static struct Queue* queue = NULL;

static gboolean on_draw_queue(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int height = gtk_widget_get_allocated_height(widget);
    
    cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.95, 0.95, 0.95);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.9, 0.9, 0.9);
    cairo_set_source(cr, pat);
    cairo_paint(cr);
    cairo_pattern_destroy(pat);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);

    int x = 50, y = height / 2, w = 60, h = 50, sp = 40;
    char buf[32];
    cairo_text_extents_t ext;

    cairo_text_extents(cr, "FRONT", &ext);
    cairo_move_to(cr, x, y - h/2 - 15);
    cairo_show_text(cr, "FRONT");
    
    cairo_move_to(cr, x + 15, y - h/2 - 10);
    cairo_line_to(cr, x + 15, y - h/2);
    cairo_stroke(cr);
    
    if(!queue || !queue->front) {
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        cairo_rectangle(cr, x, y - h/2, w, h);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
        cairo_rectangle(cr, x, y - h/2, w, h);
        cairo_stroke(cr);
        
        cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
        cairo_text_extents(cr, "NULL", &ext);
        cairo_move_to(cr, x + (w-ext.width)/2, y + (ext.height)/2);
        cairo_show_text(cr, "NULL");
    } else {
        struct Node *t = queue->front;
        while(t) {
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_rectangle(cr, x, y - h/2, w, h);
            cairo_fill(cr);
            
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_rectangle(cr, x, y - h/2, w, h);
            cairo_stroke(cr);
            
            sprintf(buf, "%d", t->data);
            cairo_text_extents(cr, buf, &ext);
            cairo_move_to(cr, x + (w-ext.width)/2, y + (ext.height)/2);
            cairo_show_text(cr, buf);

            if (t == queue->rear) {
                cairo_text_extents(cr, "REAR", &ext);
                cairo_move_to(cr, x, y + h/2 + 25);
                cairo_show_text(cr, "REAR");
                
                cairo_move_to(cr, x + 15, y + h/2 + 10);
                cairo_line_to(cr, x + 15, y + h/2);
                cairo_stroke(cr);
            }

            cairo_move_to(cr, x + w, y);
            cairo_line_to(cr, x + w + sp, y);
            cairo_move_to(cr, x + w + sp - 10, y - 5);
            cairo_line_to(cr, x + w + sp, y);
            cairo_line_to(cr, x + w + sp - 10, y + 5);
            cairo_stroke(cr);

            x += w + sp;
            t = t->next;

            if(!t) {
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                cairo_rectangle(cr, x, y - h/2, w, h);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
                cairo_rectangle(cr, x, y - h/2, w, h);
                cairo_stroke(cr);
                
                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                cairo_text_extents(cr, "NULL", &ext);
                cairo_move_to(cr, x + (w-ext.width)/2, y + (ext.height)/2);
                cairo_show_text(cr, "NULL");
            }
        }
    }
    return FALSE;
}

static void queue_act(GtkButton *b, gpointer data) {
    struct CommonControls *c = (struct CommonControls*)data;
    const char *lbl = gtk_button_get_label(b);
    int val = get_integer_input(c->value_entry);
    char msg[64];

    if(!queue) { queue = malloc(sizeof(struct Queue)); queue->front = queue->rear = NULL; }

    if (strstr(lbl, "Enqueue")) {
        if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }
        struct Node *n = malloc(sizeof(struct Node));
        n->data = val; n->next = NULL;
        if (!queue->rear) queue->front = queue->rear = n;
        else { queue->rear->next = n; queue->rear = n; }
        sprintf(msg, "Enqueued %d", val);
        gtk_entry_set_text(GTK_ENTRY(c->value_entry), "");
    } else if (strstr(lbl, "Dequeue")) {
        if (!queue->front) { update_status(c->status_label, "Queue Empty", NULL); return; }
        struct Node *t = queue->front;
        int d = t->data;
        queue->front = queue->front->next;
        if (!queue->front) queue->rear = NULL;
        free(t);
        sprintf(msg, "Dequeued %d", d);
    } else {
        if (!queue->front) { update_status(c->status_label, "Queue Empty", NULL); return; }
        sprintf(msg, "Front: %d", queue->front->data);
    }
    update_status(c->status_label, msg, c->drawing_area);
}

GtkWidget* create_queue_page() {
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

    const char* btns[] = {"Enqueue", "Dequeue", "Peek"};
    for(int i=0; i<3; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i]);
        g_signal_connect(b, "clicked", G_CALLBACK(queue_act), c);
        gtk_box_pack_start(GTK_BOX(ctrl_box), b, FALSE, FALSE, 0);
    }
    g_signal_connect(c->drawing_area, "draw", G_CALLBACK(on_draw_queue), NULL);

    c->status_label = gtk_label_new("Ready");
    gtk_label_set_use_markup(GTK_LABEL(c->status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), c->status_label, 0, 2, 1, 1);
    g_object_add_weak_pointer(G_OBJECT(grid), (gpointer *)&c);
    return grid;
}
