#include "common.h"

static struct Node* head = NULL;
static struct Node* found_node = NULL;

static gboolean on_draw_ll(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    cairo_pattern_t *pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 0.96, 0.96, 0.96);
    cairo_pattern_add_color_stop_rgb(pat, 1.0, 0.92, 0.92, 0.92);
    cairo_set_source(cr, pat);
    cairo_paint(cr);
    cairo_pattern_destroy(pat);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 14);

    int x = 120, y = height / 2;
    int w = 60, h = 50, space = 50;
    char buf[32];
    cairo_text_extents_t ext;

    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_text_extents(cr, "HEAD", &ext);
    cairo_move_to(cr, x - 80 - ext.width, y + ext.height/2);
    cairo_show_text(cr, "HEAD");
    
    cairo_move_to(cr, x - 70, y);
    cairo_line_to(cr, x, y); 
    cairo_move_to(cr, x - 10, y - 5);
    cairo_line_to(cr, x, y);
    cairo_line_to(cr, x - 10, y + 5); 
    cairo_stroke(cr);

    struct Node* curr = head;
    if (!curr) {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_rectangle(cr, x, y - h/2, w, h);
        cairo_stroke(cr);
        cairo_text_extents(cr, "NULL", &ext);
        cairo_move_to(cr, x + (w - ext.width)/2, y + (ext.height)/2);
        cairo_show_text(cr, "NULL");
    }

    while (curr) {
        if (curr == found_node) {
            cairo_set_source_rgb(cr, 0.2, 0.8, 0.2); 
            cairo_rectangle(cr, x, y - h/2, w, h);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 0, 0, 0);
        } else {
            cairo_set_source_rgb(cr, 1, 1, 1); 
            cairo_rectangle(cr, x, y - h/2, w, h);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 0, 0, 0); 
        }

        cairo_rectangle(cr, x, y - h/2, w, h);
        cairo_stroke(cr);
        
        if (curr == found_node) cairo_set_source_rgb(cr, 1, 1, 1); 
        else cairo_set_source_rgb(cr, 0, 0, 0);

        sprintf(buf, "%d", curr->data);
        cairo_text_extents(cr, buf, &ext);
        cairo_move_to(cr, x + (w - ext.width)/2, y + (ext.height)/2);
        cairo_show_text(cr, buf);

        cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
        cairo_move_to(cr, x + w, y);
        cairo_line_to(cr, x + w + space, y);
        
        cairo_move_to(cr, x + w + space - 10, y - 5);
        cairo_line_to(cr, x + w + space, y);
        cairo_line_to(cr, x + w + space - 10, y + 5);
        cairo_stroke(cr);

        x += w + space;
        curr = curr->next;

        if (!curr) {
            cairo_set_source_rgb(cr, 0.7, 0.7, 0.7); 
            cairo_rectangle(cr, x, y - h/2, w, h); 
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
            cairo_rectangle(cr, x, y - h/2, w, h);
            cairo_stroke(cr);
            
            cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
            cairo_text_extents(cr, "NULL", &ext);
            cairo_move_to(cr, x + (w - ext.width)/2, y + (ext.height)/2);
            cairo_show_text(cr, "NULL");
        }
    }
    return FALSE;
}

static void ll_act(GtkButton *b, gpointer data) {
    struct CommonControls *c = (struct CommonControls*)data;
    const char *lbl = gtk_button_get_label(b);
    int val = get_integer_input(c->value_entry);
    int pos = get_integer_input(c->pos_entry);
    char msg[64];

    if (strstr(lbl, "Search")) {
        if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }
        struct Node *t = head;
        int idx = 1;
        found_node = NULL;
        while(t) {
            if(t->data == val) {
                found_node = t;
                sprintf(msg, "Found %d at pos %d", val, idx);
                update_status(c->status_label, msg, c->drawing_area);
                return;
            }
            t=t->next; idx++;
        }
        update_status(c->status_label, "Value not found", c->drawing_area);
        return;
    }

    found_node = NULL;
    if (strstr(lbl, "Ins")) {
        if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }
        struct Node *n = malloc(sizeof(struct Node));
        n->data = val; n->next = NULL;

        if (strstr(lbl, "Head")) {
            n->next = head; head = n;
            sprintf(msg, "Inserted %d at Head", val);
        } else if (strstr(lbl, "Tail")) {
            if(!head) head = n;
            else {
                struct Node *t = head;
                while(t->next) t=t->next;
                t->next = n;
            }
            sprintf(msg, "Inserted %d at Tail", val);
        } else {
            if (pos < 1) { free(n); update_status(c->status_label, "Invalid Position", NULL); return; }
            if (pos == 1) { n->next = head; head = n; }
            else {
                struct Node *t = head;
                for(int i=1; i<pos-1 && t; i++) t=t->next;
                if(!t) { free(n); update_status(c->status_label, "Position Out of Bounds", NULL); return; }
                n->next = t->next; t->next = n;
            }
            sprintf(msg, "Inserted %d at Pos %d", val, pos);
        }
    } else if (strstr(lbl, "Del")) {
        if (!head) { update_status(c->status_label, "List Empty", NULL); return; }
        struct Node *temp = head;
        
        if (strstr(lbl, "Head")) {
            head = head->next; free(temp);
            sprintf(msg, "Deleted Head");
        } else if (strstr(lbl, "Tail")) {
            if (!head->next) { free(head); head = NULL; }
            else {
                struct Node *prev = NULL;
                while(temp->next) { prev=temp; temp=temp->next; }
                prev->next = NULL; free(temp);
            }
            sprintf(msg, "Deleted Tail");
        } else {
            if (pos < 1) { update_status(c->status_label, "Invalid Position", NULL); return; }
            if (pos == 1) { head = head->next; free(temp); }
            else {
                struct Node *prev = NULL;
                for(int i=1; i<pos && temp; i++) { prev=temp; temp=temp->next; }
                if(!temp) { update_status(c->status_label, "Position Out of Bounds", NULL); return; }
                prev->next = temp->next; free(temp);
            }
            sprintf(msg, "Deleted at Pos %d", pos);
        }
    }
    
    gtk_entry_set_text(GTK_ENTRY(c->value_entry), "");
    gtk_entry_set_text(GTK_ENTRY(c->pos_entry), "");
    update_status(c->status_label, msg, c->drawing_area);
}

GtkWidget* create_ll_page() {
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

    c->pos_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(c->pos_entry), "Pos");
    gtk_entry_set_width_chars(GTK_ENTRY(c->pos_entry), 5);
    gtk_box_pack_start(GTK_BOX(ctrl_box), c->pos_entry, FALSE, FALSE, 0);
    
    const char* btns[] = {"Ins Head", "Ins Tail", "Ins Pos", "Del Head", "Del Tail", "Del Pos", "Search"};
    for(int i=0; i<7; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i]);
        g_signal_connect(b, "clicked", G_CALLBACK(ll_act), c);
        gtk_box_pack_start(GTK_BOX(ctrl_box), b, FALSE, FALSE, 0);
    }
    g_signal_connect(c->drawing_area, "draw", G_CALLBACK(on_draw_ll), NULL);

    c->status_label = gtk_label_new("Ready");
    gtk_label_set_use_markup(GTK_LABEL(c->status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), c->status_label, 0, 2, 1, 1);
    g_object_add_weak_pointer(G_OBJECT(grid), (gpointer *)&c);
    return grid;
}
