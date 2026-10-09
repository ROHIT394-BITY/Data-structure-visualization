#ifndef COMMON_H
#define COMMON_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#include <ctype.h>
#include <math.h>

#define HASH_SIZE 10
#define ANIMATION_STEPS 20
#define ANIMATION_SPEED 20

// --- Data Structures ---
struct Node {
    int data;
    struct Node* next;
};

struct Queue {
    struct Node *front, *rear;
};

struct AvlNode {
    int data;
    struct AvlNode *left;
    struct AvlNode *right;
    int height;
    double x, y;
    double target_x, target_y;
};

// --- GUI Structures ---
struct CommonControls {
    GtkWidget *value_entry;
    GtkWidget *pos_entry;
    GtkWidget *status_label;
    GtkWidget *drawing_area;
};

// --- Helpers ---
void update_status(GtkWidget *label, const char *msg, GtkWidget *area);
int get_integer_input(GtkWidget *entry);


GtkWidget* create_ll_page();
GtkWidget* create_stack_page();
GtkWidget* create_queue_page();
GtkWidget* create_avl_page();
GtkWidget* create_hash_page();

#endif
