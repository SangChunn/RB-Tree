#include "rbtree.h"
#include <stdlib.h>

static void left_rotate(rbtree *t, node_t *x);
static void right_rotate(rbtree *t, node_t *x);
static void insert_fixup(rbtree *t, node_t *z);
static void erase_fixup(rbtree *t, node_t *x);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  p -> nil = calloc(1, sizeof(node_t));
  p -> nil -> color = RBTREE_BLACK;
  p -> nil -> parent = p -> nil -> left = p -> nil -> right = p -> nil;
  p -> root = p -> nil;
  return p;
}
static void free_subtree(const rbtree *t, node_t *x){
  if (x == t->nil) return;
  free_subtree(t, x->left);
  free_subtree(t, x->right);
  free(x);
}
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if (t == NULL) return;
  if (t -> root != t -> nil){
    free_subtree(t, t -> root);
  }
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = malloc(sizeof(node_t));
  z->key = key;
  z->color = RBTREE_RED;
  z -> left = z -> right = z -> parent = t -> nil;

  node_t *y = t->nil;
  node_t *x = t->root;
  while (x != t->nil) {
    y = x;
    x = (key < x->key) ? x->left : x->right;
  }
  z->parent = y;
  if (y == t->nil) t->root = z;
  else if (key < y->key) y->left = z;
  else y->right = z;

  insert_fixup(t, z);
  return z;
}

static void left_rotate(rbtree *t, node_t *x){
  node_t *y = x -> right;
  x -> right = y -> left;
  if (y -> left != t -> nil) y -> left -> parent = x;
  y -> parent = x -> parent;
  if(x -> parent == t -> nil)
    t -> root = y;
  else if(x == x->parent->left)
    x -> parent -> left = y;
  else  
    x -> parent -> right = y;
  y -> left = x;
  x -> parent = y;
}

static void right_rotate(rbtree *t, node_t *x){
  node_t *y = x -> left;
  x -> left = y -> right;
  if (y -> right != t -> nil)
    y -> right -> parent = x;
  y -> parent = x -> parent;
  if (x -> parent == t ->nil)
    t -> root = y;
  else if(x == x -> parent -> right)
    x -> parent -> right = y;
  else
    x -> parent -> left = y;
  y -> right = x;
  x -> parent = y;
}

static void insert_fixup(rbtree *t, node_t *z){
  while (z -> parent -> color == RBTREE_RED)
  {
    if (z -> parent == z -> parent -> parent -> left){
      node_t *y = z -> parent -> parent -> right;
      if (y -> color == RBTREE_RED){
        z -> parent -> color = RBTREE_BLACK;
        y -> color = RBTREE_BLACK;
        z -> parent -> parent -> color = RBTREE_RED;
        z = z -> parent -> parent;
      }
      else if (y -> color == RBTREE_BLACK){
        if (z == z -> parent -> right){
          z = z -> parent;
          left_rotate(t, z);
        }
        z -> parent -> color = RBTREE_BLACK;
        z -> parent -> parent -> color = RBTREE_RED;
        right_rotate(t, z -> parent -> parent);
      }
    }else{
      node_t *y = z->parent->parent->left;             
        if (y->color == RBTREE_RED) {                    
          z->parent->color = RBTREE_BLACK;
          y->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          z = z->parent->parent;
        }
        else if(y -> color == RBTREE_BLACK){
          if (z == z -> parent -> left){
            z = z -> parent;
            right_rotate(t, z);
          }
          z -> parent -> color = RBTREE_BLACK;
          z -> parent -> parent -> color = RBTREE_RED;
          left_rotate(t, z -> parent -> parent);
        }
    }
  }
  t -> root -> color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *cur = t -> root;
  while (cur != t -> nil)
  {
    if (key == cur -> key)
      return cur;
    else if(key < cur -> key)
      cur = cur -> left;
    else
      cur = cur -> right;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *cur = t -> root;

  if (cur == t -> nil)
    return t -> nil;
  while (cur -> left != t -> nil)
    cur = cur -> left;
  return cur;
}

node_t *rbtree_max(const rbtree *t) {                                                                                          
  // TODO: implement find
  node_t *cur = t -> root;

  if (cur == t -> nil)
    return t -> nil;
  while(cur -> right != t -> nil)
    cur = cur -> right;
  return cur;
}

static void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if (u -> parent == t -> nil)
    t -> root = v;
  else if(u == u -> parent -> left)
    u -> parent -> left =  v;
  else  
    u -> parent -> right = v;
  
  if (v != t->nil) v -> parent = u -> parent;
}

static node_t *tree_minimum(const rbtree *t, node_t *x){
  while (x -> left != t -> nil)
    x = x -> left;
  return x;
}

int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t *y = z;
  color_t y_basic_color = y->color;
  node_t *x;

  if (z->left == t->nil){
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil){
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else{
    y = tree_minimum(t, z->right);
    y_basic_color = y->color;
    x = y->right;

    if (y->parent == z){
      if (x != t->nil) x->parent = y;
    }else{
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (y_basic_color == RBTREE_BLACK){
    erase_fixup(t, x);
  }
  free(z);
  return 0;
}

static void erase_fixup(rbtree *t, node_t *x){
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left){
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else{
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK &&           
          w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        if (w->left->color == RBTREE_BLACK) {           
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;                    
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

static void inorder_fill (const rbtree *t, node_t *cur, key_t *arr, size_t n, size_t *idx){
  if (cur == t->nil || *idx >= n) return;

  inorder_fill(t, cur->left, arr, n , idx);
  arr[(*idx)++] = cur->key;
  inorder_fill(t, cur->right, arr, n, idx);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  size_t idx = 0;
  inorder_fill(t, t->root, arr, n, &idx);
  return 0;
}
