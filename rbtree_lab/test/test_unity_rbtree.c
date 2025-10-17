// test/test_unity_rbtree.c
#include "unity.h"
#include "rbtree.h"
#include <stdlib.h>
#include <stdbool.h>

/* ---------- 유틸 ---------- */

static void insert_arr(rbtree *t, const key_t *arr, size_t n) {
  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    TEST_ASSERT_NOT_NULL_MESSAGE(p, "insert returned NULL");
  }
}

static int cmp_key(const void *a, const void *b) {
  key_t x = *(const key_t*)a;
  key_t y = *(const key_t*)b;
  return (x > y) - (x < y);
}

/* ---------- 색 제약/검색 트리 제약 검사용 DFS ---------- */

static bool touch_nil;
static int  max_black_depth;

static void init_color_traverse(void) {
  touch_nil = false;
  max_black_depth = 0;
}

static bool color_traverse(const node_t *p, color_t parent_color, int black_depth, node_t *nil) {
  if (p == nil) {
    if (!touch_nil) {
      touch_nil = true;
      max_black_depth = black_depth;
    } else if (black_depth != max_black_depth) {
      return false;
    }
    return true;
  }
  if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
    return false; // red-red 위반
  }
  int next_depth = black_depth + ((p->color == RBTREE_BLACK) ? 1 : 0);
  return color_traverse(p->left,  p->color, next_depth, nil) &&
         color_traverse(p->right, p->color, next_depth, nil);
}

static bool search_traverse(const node_t *p, key_t *min, key_t *max, node_t *nil) {
  if (p == nil) {
    return true;
  }
  *min = *max = p->key;

  key_t lmin = p->key, lmax = p->key, rmin = p->key, rmax = p->key;
  if (!search_traverse(p->left, &lmin, &lmax, nil)) return false;
  if (lmax > p->key) return false;
  if (!search_traverse(p->right, &rmin, &rmax, nil)) return false;
  if (rmin < p->key) return false;

  *min = lmin;
  *max = rmax;
  return true;
}

/* ---------- Unity 훅 ---------- */

void setUp(void)   {}
void tearDown(void){}

/* ---------- 테스트들 ---------- */

static void test_new_rbtree_init(void) {
  rbtree *t = new_rbtree();
  TEST_ASSERT_NOT_NULL(t);
#ifdef SENTINEL
  TEST_ASSERT_NOT_NULL(t->nil);
  TEST_ASSERT_EQUAL_PTR(t->nil, t->root);
#else
  TEST_ASSERT_EQUAL_PTR(NULL, t->root);
#endif
  delete_rbtree(t);
}

static void test_insert_single(void) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, 1024);
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_PTR(t->root, p);
  TEST_ASSERT_EQUAL_INT(1024, p->key);
#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(t->nil, p->left);
  TEST_ASSERT_EQUAL_PTR(t->nil, p->right);
  TEST_ASSERT_EQUAL_PTR(t->nil, p->parent);
#endif
  delete_rbtree(t);
}

static void test_find_single(void) {
  rbtree *t = new_rbtree();
  node_t *ins = rbtree_insert(t, 512);
  TEST_ASSERT_NOT_NULL(ins);

  node_t *hit = rbtree_find(t, 512);
  TEST_ASSERT_NOT_NULL(hit);
  TEST_ASSERT_EQUAL_INT(512, hit->key);
  TEST_ASSERT_EQUAL_PTR(ins, hit);

  node_t *miss = rbtree_find(t, 999);
  TEST_ASSERT_NULL(miss);

  delete_rbtree(t);
}

static void test_erase_root(void) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, 128);
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_PTR(t->root, p);

  TEST_ASSERT_EQUAL_INT(0, rbtree_erase(t, p));
#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(t->nil, t->root);
#else
  TEST_ASSERT_NULL(t->root);
#endif
  delete_rbtree(t);
}

static void test_minmax_basic(void) {
  key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(arr)/sizeof(arr[0]);

  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);

#ifdef SENTINEL
  TEST_ASSERT(t->root != t->nil);
#else
  TEST_ASSERT_NOT_NULL(t->root);
#endif

  qsort(arr, n, sizeof(key_t), cmp_key);

  node_t *mn = rbtree_min(t);
  node_t *mx = rbtree_max(t);
  TEST_ASSERT_NOT_NULL(mn);
  TEST_ASSERT_NOT_NULL(mx);
  TEST_ASSERT_EQUAL_INT(arr[0], mn->key);
  TEST_ASSERT_EQUAL_INT(arr[n-1], mx->key);

  // min 삭제 후 다음 min 확인
  TEST_ASSERT_EQUAL_INT(0, rbtree_erase(t, mn));
  mn = rbtree_min(t);
  TEST_ASSERT_NOT_NULL(mn);
  TEST_ASSERT_EQUAL_INT(arr[1], mn->key);

  // max 삭제 후 다음 max 확인
  if (n >= 2) {
    TEST_ASSERT_EQUAL_INT(0, rbtree_erase(t, mx));
    mx = rbtree_max(t);
    TEST_ASSERT_NOT_NULL(mx);
    TEST_ASSERT_EQUAL_INT(arr[n-2], mx->key);
  }
  delete_rbtree(t);
}

static void test_to_array_sorted(void) {
  rbtree *t = new_rbtree();
  key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(arr)/sizeof(arr[0]);

  insert_arr(t, arr, n);
  qsort(arr, n, sizeof(key_t), cmp_key);

  key_t *out = (key_t*)calloc(n, sizeof(key_t));
  TEST_ASSERT_NOT_NULL(out);

  TEST_ASSERT_EQUAL_INT(0, rbtree_to_array(t, out, n));
  for (size_t i = 0; i < n; i++) {
    TEST_ASSERT_EQUAL_INT(arr[i], out[i]);
  }
  free(out);
  delete_rbtree(t);
}

static void test_rb_constraints_distinct(void) {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(arr)/sizeof(arr[0]);

  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);

#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif

  // root는 black이어야 함 (빈 트리가 아니면)
  if (t->root != nil) {
    TEST_ASSERT_EQUAL_INT(RBTREE_BLACK, t->root->color);
  }

  // 색 제약
  init_color_traverse();
  TEST_ASSERT_TRUE(color_traverse(t->root, RBTREE_BLACK, 0, nil));

  // 검색 트리 제약
  key_t minK, maxK;
  TEST_ASSERT_TRUE(search_traverse(t->root, &minK, &maxK, nil));

  delete_rbtree(t);
}

static void test_rb_constraints_with_dups(void) {
  const key_t arr[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
  const size_t n = sizeof(arr)/sizeof(arr[0]);

  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);

#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif

  if (t->root != nil) {
    TEST_ASSERT_EQUAL_INT(RBTREE_BLACK, t->root->color);
  }

  init_color_traverse();
  TEST_ASSERT_TRUE(color_traverse(t->root, RBTREE_BLACK, 0, nil));

  key_t minK, maxK;
  TEST_ASSERT_TRUE(search_traverse(t->root, &minK, &maxK, nil));

  delete_rbtree(t);
}

static void test_find_erase_roundtrip_fixed(void) {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(arr)/sizeof(arr[0]);

  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);

  // find 모두 확인 & erase
  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(arr[i], p->key);
    TEST_ASSERT_EQUAL_INT(0, rbtree_erase(t, p));
  }
  // 모두 사라졌는지 확인
  for (size_t i = 0; i < n; i++) {
    TEST_ASSERT_NULL(rbtree_find(t, arr[i]));
  }

  // 다시 넣고 바로 지우는 라운드트립
  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    TEST_ASSERT_NOT_NULL(p);
    node_t *q = rbtree_find(t, arr[i]);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_PTR(p, q);
    TEST_ASSERT_EQUAL_INT(0, rbtree_erase(t, p));
    TEST_ASSERT_NULL(rbtree_find(t, arr[i]));
  }

  delete_rbtree(t);
}

static void test_multi_instance(void) {
  rbtree *a = new_rbtree();
  rbtree *b = new_rbtree();
  TEST_ASSERT_NOT_NULL(a);
  TEST_ASSERT_NOT_NULL(b);

  key_t arrA[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  key_t arrB[] = {4, 8, 10, 5, 3};
  const size_t nA = sizeof(arrA)/sizeof(arrA[0]);
  const size_t nB = sizeof(arrB)/sizeof(arrB[0]);

  insert_arr(a, arrA, nA);
  insert_arr(b, arrB, nB);

  // 각각 to_array가 정렬 결과를 내는지만 확인
  qsort(arrA, nA, sizeof(key_t), cmp_key);
  qsort(arrB, nB, sizeof(key_t), cmp_key);

  key_t *outA = (key_t*)calloc(nA, sizeof(key_t));
  key_t *outB = (key_t*)calloc(nB, sizeof(key_t));
  TEST_ASSERT_NOT_NULL(outA);
  TEST_ASSERT_NOT_NULL(outB);

  TEST_ASSERT_EQUAL_INT(0, rbtree_to_array(a, outA, nA));
  TEST_ASSERT_EQUAL_INT(0, rbtree_to_array(b, outB, nB));
  for (size_t i = 0; i < nA; i++) TEST_ASSERT_EQUAL_INT(arrA[i], outA[i]);
  for (size_t i = 0; i < nB; i++) TEST_ASSERT_EQUAL_INT(arrB[i], outB[i]);

  free(outA); free(outB);
  delete_rbtree(b);
  delete_rbtree(a);
}

/* ---------- 메인 ---------- */

int main(void) {
  UnityBegin("test_unity_rbtree.c");

  RUN_TEST(test_new_rbtree_init);
  RUN_TEST(test_insert_single);
  RUN_TEST(test_find_single);
  RUN_TEST(test_erase_root);
  RUN_TEST(test_minmax_basic);
  RUN_TEST(test_to_array_sorted);
  RUN_TEST(test_rb_constraints_distinct);
  RUN_TEST(test_rb_constraints_with_dups);
  RUN_TEST(test_find_erase_roundtrip_fixed);
  RUN_TEST(test_multi_instance);

  return UnityEnd();
}