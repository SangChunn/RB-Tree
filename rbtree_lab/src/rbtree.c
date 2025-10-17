#include "rbtree.h"

#include <stdlib.h>

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

  node_t *cur = t -> root;
  node_t *parent = t-> nil;
  while (cur != t -> nil){
    parent = cur;

    if (key < cur -> key)
      cur = cur -> left;
    else
      cur = cur -> right;
  }
  z -> parent = parent;
  if (parent == t -> nil)
    t -> root = z;
  else if(z->key < parent->key)
    parent->left = z;
  else
    parent->right = z;
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
