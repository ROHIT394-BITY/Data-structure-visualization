#include "common.h"

static struct Node* hash_table[HASH_SIZE];
static int hash_found_index = -1;
static int hash_found_value = -1;

static gboolean on_draw_hash(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
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

    int bw=60, bh=40, vs=10, cx=150, nw=50, nh=40, hs=30;
    char buf[32];
    cairo_text_extents_t ext;

    for (int i=0; i<HASH_SIZE; i++) {
        int x=50, y=30 + i*(bh+vs);
        if (i == hash_found_index) cairo_set_source_rgb(cr, 0.5, 0.8, 1.0);
        else cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
        cairo_rectangle(cr, x, y, bw, bh);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, x, y, bw, bh);
        cairo_stroke(cr);

        sprintf(buf, "[%d]", i);
        cairo_text_extents(cr, buf, &ext);
        cairo_move_to(cr, x + (bw-ext.width)/2, y + (bh+ext.height)/2);
        cairo_show_text(cr, buf);

        cairo_move_to(cr, x+bw, y+bh/2);
        cairo_line_to(cr, cx, y+bh/2);
        cairo_stroke(cr);

        struct Node *t = hash_table[i];
        int cur_x = cx;
        if (!t) {
            cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
            cairo_rectangle(cr, cur_x, y, nw, nh);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
            cairo_rectangle(cr, cur_x, y, nw, nh);
            cairo_stroke(cr);
            
            cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
            cairo_text_extents(cr, "NULL", &ext);
            cairo_move_to(cr, cur_x + (nw-ext.width)/2, y + (nh+ext.height)/2);
            cairo_show_text(cr, "NULL");
        }
        while(t) {
            if (i == hash_found_index && t->data == hash_found_value) cairo_set_source_rgb(cr, 0.2, 0.8, 0.2);
            else cairo_set_source_rgb(cr, 1, 1, 1);
            
            cairo_rectangle(cr, cur_x, y, nw, nh);
            cairo_fill(cr);
            
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_rectangle(cr, cur_x, y, nw, nh);
            cairo_stroke(cr);
            
            if (i == hash_found_index && t->data == hash_found_value) cairo_set_source_rgb(cr, 1, 1, 1);
            else cairo_set_source_rgb(cr, 0, 0, 0);
            
            sprintf(buf, "%d", t->data);
            cairo_text_extents(cr, buf, &ext);
            cairo_move_to(cr, cur_x + (nw-ext.width)/2, y + (nh+ext.height)/2);
            cairo_show_text(cr, buf);

            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_move_to(cr, cur_x+nw, y+nh/2);
            cairo_line_to(cr, cur_x+nw+hs, y+nh/2);
            cairo_stroke(cr);

            cur_x += nw+hs;
            t = t->next;
            if(!t) {
                cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
                cairo_rectangle(cr, cur_x, y, nw, nh);
                cairo_fill(cr);
                cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
                cairo_rectangle(cr, cur_x, y, nw, nh);
                cairo_stroke(cr);
                
                cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
                cairo_text_extents(cr, "NULL", &ext);
                cairo_move_to(cr, cur_x + (nw-ext.width)/2, y + (nh+ext.height)/2);
                cairo_show_text(cr, "NULL");
            }
        }
    }
    return FALSE;
}

static void hash_act(GtkButton *b, gpointer data) {
    struct CommonControls *c = (struct CommonControls*)data;
    const char *lbl = gtk_button_get_label(b);
    int val = get_integer_input(c->value_entry);
    char msg[64];

    if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }
    int idx = val % HASH_SIZE;
    if (idx < 0) idx += HASH_SIZE;

    if (strstr(lbl, "Insert")) {
        struct Node *n = malloc(sizeof(struct Node));
        n->data = val; n->next = hash_table[idx]; hash_table[idx] = n;
        sprintf(msg, "Inserted %d at [%d]", val, idx);
        hash_found_index = -1;
    } else {
        struct Node *t = hash_table[idx];
        hash_found_index = -1;
        while(t) {
            if (t->data == val) {
                hash_found_index = idx; hash_found_value = val;
                sprintf(msg, "Found %d at [%d]", val, idx);
                goto end;
            }
            t=t->next;
        }
        sprintf(msg, "Value %d not found", val);
    }
    end:
    gtk_entry_set_text(GTK_ENTRY(c->value_entry), "");
    update_status(c->status_label, msg, c->drawing_area);
}

GtkWidget* create_hash_page() {
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

    const char* btns[] = {"Insert", "Search"};
    for(int i=0; i<2; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i]);
        g_signal_connect(b, "clicked", G_CALLBACK(hash_act), c);
        gtk_box_pack_start(GTK_BOX(ctrl_box), b, FALSE, FALSE, 0);
    }
    g_signal_connect(c->drawing_area, "draw", G_CALLBACK(on_draw_hash), NULL);

    c->status_label = gtk_label_new("Ready");
    gtk_label_set_use_markup(GTK_LABEL(c->status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), c->status_label, 0, 2, 1, 1);
    g_object_add_weak_pointer(G_OBJECT(grid), (gpointer *)&c);
    return grid;
}
