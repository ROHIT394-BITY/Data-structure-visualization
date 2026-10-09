#include "common.h"

static struct AvlNode* avl_root = NULL;
static gboolean is_animating = FALSE;
static int animation_step = 0;
static guint animation_timer_id = 0;

int avl_h(struct AvlNode *n) { return n ? n->height : 0; }
int avl_max(int a, int b) { return (a > b) ? a : b; }
struct AvlNode* avl_new(int d) {
    struct AvlNode* n = malloc(sizeof(struct AvlNode));
    n->data = d; n->left = n->right = NULL; n->height = 1;
    n->x = n->y = 0;
    n->target_x = n->target_y = 0;
    return n;
}

struct AvlNode *avl_rrot(struct AvlNode *y) {
    struct AvlNode *x = y->left;
    struct AvlNode *T2 = x->right;
    x->right = y; y->left = T2;
    y->height = avl_max(avl_h(y->left), avl_h(y->right)) + 1;
    x->height = avl_max(avl_h(x->left), avl_h(x->right)) + 1;
    return x;
}

struct AvlNode *avl_lrot(struct AvlNode *x) {
    struct AvlNode *y = x->right;
    struct AvlNode *T2 = y->left;
    y->left = x; x->right = T2;
    x->height = avl_max(avl_h(x->left), avl_h(x->right)) + 1;
    y->height = avl_max(avl_h(y->left), avl_h(y->right)) + 1;
    return y;
}

int avl_bal(struct AvlNode *n) { return n ? avl_h(n->left) - avl_h(n->right) : 0; }

struct AvlNode* avl_ins(struct AvlNode* n, int d) {
    if (!n) return avl_new(d);
    if (d < n->data) n->left = avl_ins(n->left, d);
    else if (d > n->data) n->right = avl_ins(n->right, d);
    else return n;

    n->height = 1 + avl_max(avl_h(n->left), avl_h(n->right));
    int bal = avl_bal(n);

    if (bal > 1 && d < n->left->data) return avl_rrot(n);
    if (bal < -1 && d > n->right->data) return avl_lrot(n);
    if (bal > 1 && d > n->left->data) { n->left = avl_lrot(n->left); return avl_rrot(n); }
    if (bal < -1 && d < n->right->data) { n->right = avl_rrot(n->right); return avl_lrot(n); }
    return n;
}

struct AvlNode* avl_min(struct AvlNode* n) {
    struct AvlNode* c = n;
    while (c->left) c = c->left;
    return c;
}

struct AvlNode* avl_del(struct AvlNode* root, int d) {
    if (!root) return root;
    if (d < root->data) root->left = avl_del(root->left, d);
    else if (d > root->data) root->right = avl_del(root->right, d);
    else {
        if (!root->left || !root->right) {
            struct AvlNode *temp = root->left ? root->left : root->right;
            if (!temp) { temp = root; root = NULL; }
            else *root = *temp;
            free(temp);
        } else {
            struct AvlNode* temp = avl_min(root->right);
            root->data = temp->data;
            root->right = avl_del(root->right, temp->data);
        }
    }
    if (!root) return root;
    root->height = 1 + avl_max(avl_h(root->left), avl_h(root->right));
    int bal = avl_bal(root);
    if (bal > 1 && avl_bal(root->left) >= 0) return avl_rrot(root);
    if (bal > 1 && avl_bal(root->left) < 0) { root->left = avl_lrot(root->left); return avl_rrot(root); }
    if (bal < -1 && avl_bal(root->right) <= 0) return avl_lrot(root);
    if (bal < -1 && avl_bal(root->right) > 0) { root->right = avl_rrot(root->right); return avl_lrot(root); }
    return root;
}

void calculate_positions(struct AvlNode *root, double x, double y, double h_spacing) {
    if (!root) return;
    root->target_x = x;
    root->target_y = y;
    if (root->left) calculate_positions(root->left, x - h_spacing, y + 80, h_spacing / 2);
    if (root->right) calculate_positions(root->right, x + h_spacing, y + 80, h_spacing / 2);
}

void update_positions(struct AvlNode *root, double progress) {
    if (!root) return;
    if (root->x == 0 && root->y == 0) {
        root->x = root->target_x;
        root->y = root->target_y;
    } else {
        root->x += (root->target_x - root->x) * progress;
        root->y += (root->target_y - root->y) * progress;
    }
    update_positions(root->left, progress);
    update_positions(root->right, progress);
}

void draw_node_at(cairo_t *cr, struct AvlNode *node) {
    if (!node) return;
    int w = 50, h = 40;
    char buf[32];
    cairo_text_extents_t ext;

    cairo_set_source_rgb(cr, 0, 0, 0);
    if (node->left) {
        cairo_move_to(cr, node->x, node->y + h/2);
        cairo_line_to(cr, node->left->x, node->left->y - h/2);
        cairo_stroke(cr);
    }
    if (node->right) {
        cairo_move_to(cr, node->x, node->y + h/2);
        cairo_line_to(cr, node->right->x, node->right->y - h/2);
        cairo_stroke(cr);
    }

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, node->x - w/2, node->y - h/2, w, h);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, node->x - w/2, node->y - h/2, w, h);
    cairo_stroke(cr);

    sprintf(buf, "%d", node->data);
    cairo_text_extents(cr, buf, &ext);
    cairo_move_to(cr, node->x - ext.width/2, node->y + ext.height/2);
    cairo_show_text(cr, buf);
}

void draw_avl_anim(cairo_t *cr, struct AvlNode *root) {
    if (!root) return;
    draw_node_at(cr, root);
    draw_avl_anim(cr, root->left);
    draw_avl_anim(cr, root->right);
}

static gboolean on_animation_tick(gpointer user_data) {
    struct CommonControls *c = (struct CommonControls*)user_data;
    
    if (animation_step >= ANIMATION_STEPS) {
        is_animating = FALSE;
        animation_step = 0;
        animation_timer_id = 0;
        return FALSE; 
    }

    animation_step++;
    
    update_positions(avl_root, 0.1); 

    gtk_widget_queue_draw(c->drawing_area);
    return TRUE; 
}

static gboolean on_draw_avl(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
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

    if (!avl_root) {
        cairo_text_extents_t ext;
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        cairo_text_extents(cr, "Tree Empty", &ext);
        cairo_move_to(cr, (w-ext.width)/2, 100);
        cairo_show_text(cr, "Tree Empty");
    } else {
        if (!is_animating) {
            calculate_positions(avl_root, w/2, 60, w/4);
            update_positions(avl_root, 1.0); 
        }
        draw_avl_anim(cr, avl_root);
    }
    return FALSE;
}

static void avl_act(GtkButton *b, gpointer data) {
    struct CommonControls *c = (struct CommonControls*)data;
    if (is_animating) return; 

    const char *lbl = gtk_button_get_label(b);
    int val = get_integer_input(c->value_entry);
    char msg[64];
    
    if (val == -999999) { update_status(c->status_label, "Invalid Value", NULL); return; }

    if (strstr(lbl, "Insert")) {
        avl_root = avl_ins(avl_root, val);
        sprintf(msg, "Inserted %d", val);
    } else {
        avl_root = avl_del(avl_root, val);
        sprintf(msg, "Deleted %d", val);
    }
    
    int w = gtk_widget_get_allocated_width(c->drawing_area);
    calculate_positions(avl_root, w/2, 60, w/4);
    
    is_animating = TRUE;
    animation_step = 0;
    if (animation_timer_id) g_source_remove(animation_timer_id);
    animation_timer_id = g_timeout_add(ANIMATION_SPEED, on_animation_tick, c);

    gtk_entry_set_text(GTK_ENTRY(c->value_entry), "");
    update_status(c->status_label, msg, NULL);
}

GtkWidget* create_avl_page() {
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

    const char* btns[] = {"Insert", "Delete"};
    for(int i=0; i<2; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i]);
        g_signal_connect(b, "clicked", G_CALLBACK(avl_act), c);
        gtk_box_pack_start(GTK_BOX(ctrl_box), b, FALSE, FALSE, 0);
    }
    g_signal_connect(c->drawing_area, "draw", G_CALLBACK(on_draw_avl), NULL);

    c->status_label = gtk_label_new("Ready");
    gtk_label_set_use_markup(GTK_LABEL(c->status_label), TRUE);
    gtk_grid_attach(GTK_GRID(grid), c->status_label, 0, 2, 1, 1);
    g_object_add_weak_pointer(G_OBJECT(grid), (gpointer *)&c);
    return grid;
}
