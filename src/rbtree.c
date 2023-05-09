#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));  // rbtree 1개 만큼의 동적 메모리를 할당시키고 void 형태로 반환되기 때문에 (rbtree *)로 rbtree 크기만큼의 주소로 바꾼다.
                                                     // 만들어진 포인터 p는 생성된 rbtree 1개의 시작 주소값을 가지게 된다.(rbtree 1개의 시작 주소를 가르키게 된다.)
  // TODO: initialize struct if needed
  p->nil = (node_t *)calloc(1, sizeof(node_t));       // 이 때 nil의 color를 제외하곤 나머지 필드들, 즉 key, parent, left, right은 사용할 일이 없으므로 따로 값을 넣지 않아도 된다.
  p->root = p->nil;                                   // 처음 root의 값은 NULL이다. 하지만 NULL 대신 nil을 쓰기로 하였고, nil을 사용하면 color를 편하게 지정할 수 있으므로 nil을 사용하겠다.(nil->color = RBTREE_BLACK)
  p->nil->color = RBTREE_BLACK;                       // 레드 블랙 트리의 특징에 따라서 nil의 color는 black이다.
  return p;
}

// 동적 메모리 할당된 노드들의 메모리 할당 해제
void delete_node(rbtree *t, node_t *x) {
  // 현재의 노드가 트리의 nil노드와 다르면
  if (x != t->nil) {
    delete_node(t, x->left);      // 왼쪽으로 재귀
    delete_node(t, x->right);     // 오른쪽으로 재귀
    free(x);                      // 현재 노드가 가리키는 공간 할당 해제
  }
}

// 트리, 트리의 nil이 가리키는 공간 해제
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  delete_node(t, t->root);        // 생성된 노드들이 가리키는 공간 할당 해제
  free(t->nil);                   // 트리의 nil노드가 가리키는 공간 할당 해제
  
  free(t);                        // 트리가 가르키는 공간 할당 해제
}

// 회전 left
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  // 1. rotate되어 한칸 올라가는 y의 왼쪽 자식을 한칸 내려가는 x의 오른쪽 자식으로 옮긴다
  x->right = y->left;              // rotate 되어 위로 올라갈 y의 왼쪽 자식을 한칸 내려가는 x의 오른쪽 자식으로 붙여준다

  // 2.옮기고자 하는 y의 왼쪽 자식이 nil이 아니라면 부모 설정하기
  if (y->left != t->nil) {
    y->left->parent = x;
  } 

  // 3.y의 부모를 x의 부모로 바꾼다
  y->parent = x->parent;

  // 4.x의 부모의 왼쪽 혹은 오른쪽 자식에 y가 오도록 한다
  if (x->parent == t->nil) {            // x의 부모가 nil일 경우는 x가 root 노드라는 뜻이므로 root를 y로 바꿔준다
    t->root = y;
  } else if (x == x->parent->left) {    // x가 부모의 왼쪽 자식이었다면 부모의 왼쪽 자식을 y로 바꿔준다  
    x->parent->left = y;      
  } else {                              // x가 부모의 오른쪽 자식이라면 부모의 오른쪽 자식을 y로 바꿔준다
    x->parent->right = y;
  }

  // 5. 한칸 올라간 y의 왼쪽 자식이 한칸 내려간 x가 되도록 한다.
  y->left = x;              
  // 6. 5번과 연관지어, 한칸 내려간 x의 부모가 한칸 올라간 y가 되도록 한다.
  x->parent = y;

}


// 회전 right
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;

  x->left = y->right;

  if (y->right != t->nil) {
    y->right->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  } else if (x->parent->right == x){
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}

// 새로 삽입할 노드 z가 삽이된 이후 fixup이 시작된다
// z의 색은 red
// 1. z가 root일 경우 black
// 2. z의 parent color == black 아무일도 생기지 않는다
// 3. z의 parent color == red 일 때
//    3.1 z의 uncle color == red 이면 parent 와 uncle 의 color를 black으로 grandParent를 red로 바꿔준다
//    3.2 z의 uncle color == black 일때
//      3.2.1 z->parent->grandParent 가 linear일때 rotate 시켜준다. parent가 root가 되어 z와 grandParent 의 부모가 되고 uncle은 grand parent의 자식이 된다. parent는 black로 grandParent 는 red로 바꿔준다
//      3.2.2 z->parent->grandParent 가 triangle일때 z->parent 를 roate시켜 linear한 형태로 만들어 준 후 3.2.1을 수행한다 
void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    // z의 parent가 grandParent의 왼쪽 자식일 경우
    if (z->parent == z->parent->parent->left) {
      node_t *uncle = z->parent->parent->right;

      // 3.1에 해당하는 경우로 uncle이 red면 색만 조정 해주면 된다
      if (uncle->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;

      } else {  // 3.2에 해당하는 경우
        // triangle일 경우 linear로 만들어준다 
        if (z == z->parent->right) {        // z가 부모의 오른쪽 자식 이었다면 부모가 grandParent의 왼쪽 자식 이었으므로 triangle에 해당
          z = z->parent;                    // z의 부모를 축으로 삼아 
          left_rotate(t,z);                 // 왼쪽으로 회전
        }
        
        // linear일 경우 색 조정 후 rotate
        // 색은 해당 트리의 root가 될 z의 parent를 black로, z의 사촌이 될 grandParent를 red로 바꿔준다
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      } 
    } else {    // z의 parent가 grandParent의 왼쪽 자식일 경우
      node_t *uncle = z->parent->parent->left;
      
        if (uncle->color == RBTREE_RED) {
          z->parent->color = RBTREE_BLACK;
          uncle->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            right_rotate(t, z);
          }
          
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          left_rotate(t, z->parent->parent);
        }
    }
  }

  // while문을 한번도 거치지 않았다는 것은, 새로 삽입할 노드 z의 parent가 red가 아니었음을 말한다.
  // z는 이미 rbtree_insert 함수에서 root 노드가 되어 있을 것이다. 따라서 밑의 구문을 만나 black으로 바뀌게 된다.
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *x = t->root;        // 삽입할 위치를 탐색하는 포인터
  node_t *y = t->nil;         // parent를 저장해두는 포인터

  while (x != t->nil) {       // nil이 아니라면 계속 찾고, nil이라면 root부터 비어있다는 뜻이다. 
                              // 그리고 while문을 한 번도 돌지 않기 때문에 x는 주소값으로 t->nil을 가지고 있게 된다.
    y = x;                    // y에는 미리 x의 값(주소값)을 저장해놓는다. parent를 저장해놓기 위해서
    if (x->key > key) {       // 삽입하려는 값이 현재 x가 가르키는 노드의 키 값보다 작다면 왼쪽으로 탐색
      x = x->left;
    } else {
      x = x->right;           // 삽입하려는 값이 현재 x가 가르키는 노드의 키 값보다 크거나 같다면 오른쪽으로 탐색한다.
    }
  }

  // while 문을 다 돌게 되면 x는 nil을 가르키게 되었다는 뜻으로, parent인 y에 새로운 노드인 z를 삽입하면 된다.
  // 새로운 노드 z 생성(할당)
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  z->parent = y;

  if (y == t->nil) {                // while문을 1번도 돌지 않았다면 y, 즉 parent는 nil일 것이다.
    t->root = z;                    // while문을 1번도 돌지 않았다는 것은 애초에 트리가 비어있다는 뜻으로, 새로 생성한 노드 z가 rbtree의 root로 된다.
  } else if (z->key < y ->key) {    // 만약 새로 삽입할 z의 키 값이 parent의 키 값보다 작다면 parent(y)의 왼쪽에 넣어야 한다.
    y->left = z;
  } else {                          // 새로 삽입할 z의 키 값이 parent의 키 값보다 크다면 parent(y)의 오른쪽에 넣어야 한다.
    y->right = z;                       
  }

  // 새로 삽입할 노드 z는 단말 노드(리프 노드, leaf node)이기 때문에 항상 left, right는 nil이다.
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t,z);            // 특성이 위반되는 경우를 보완해주는 함수 호출

  return t->root;
}

// [노드 탐색]
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  if (t->root == t->nil) {
    return NULL;                                      
  }
  node_t *x = t->root;

  while (x != t->nil) {
    if (x->key > key) {
      x = x->left;
    } else if (x->key < key) {
      x = x->right;
    } else {
      return x;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
    node_t *x = t->root;
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;
}

// 이식
// 삭제하고자 하는 노드를 대체할 노드를 찾아, 삭제하고자 하는 노드에 대입할 동작을 수행하는 함수
// u 자리에 v 노드를 심기
// u의 부모가 가르키는 것이 v가 된다.
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {          // u의 부모가 nil일 때, 즉, 삭제할 노드가 트리의 root 일때
    t->root = v;                      // 트리의 root는 v
  } else if (u == u->parent->left) {  // u가 u의 부모의 왼쪽 자식일 때 
    u->parent->left = v;              // u의 부모의 왼쪽 자식 v
  } else {                            // u가 u의 부모의 오른쪽 자식일 때
    u->parent->right = v;             // u의 부모의 오른쪽 자식은 v
  }
  v->parent = u->parent;              // v의 부모는 u의 부모
  return;
}

void rbtree_erase_fixup(rbtree *t, node_t *x){
    node_t *w;                                                                         
    while ((x != t->root) && (x->color == RBTREE_BLACK)) {                                // 삭제한 노드의 자식이 루트가 아니고 색깔이 black일때 반복               
        if (x == x->parent->left) {                                                       // 삭제한 노드의 자식이 삭제한 노드의 부모의 왼쪽 자식일 때
            w = x->parent->right;                                                         // w는 x의 오른쪽 형제
            if (w->color == RBTREE_RED) {                                                 // type1 - w의 색깔이 red일때 (type2~4 중 해결책 찾기)  
                w->color = RBTREE_BLACK;                                                      // w의 색깔이 black으로 변경
                x->parent->color = RBTREE_RED;                                                // x의 부모의 색을 red로 변경
                left_rotate(t, x->parent);                                                    // x의 부모를 기준으로 left rotate
                w = x->parent->right;                                                   
            }                                                                           
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {      // type2 - w의 왼쪽과 오른쪽 자식이 black 일 때(type1~4 중 해결책 찾기)  
                w->color = RBTREE_RED;                                                        // w의 색깔을 red로 변경
                x = x->parent;                                                                // x의 부모를 x로 변경
            }  else {
                if (w->right->color == RBTREE_BLACK) {                                    // type3 - w의 오른쪽 자식이 black 일 때(type4 로 해결)  
                    w->left->color = RBTREE_BLACK;                                            // w의 왼쪽 색이 black으로 변경
                    w->color = RBTREE_RED;                                                    // w의 색을 red로 변경
                    right_rotate(t, w);                                                       // w를 기준으로 우회전
                    w = x->parent->right;                                                     // w는 x의 부모의 오른쪽 자식이 된다
                }
                w->color = x->parent->color;                                              // type4 - w의 오른쪽 자식이 red 일 때 w의 색을 x의 부모 색깔로 변경 
                x->parent->color = RBTREE_BLACK;                                              // x 부모의 색을 black으로 변경
                w->right->color = RBTREE_BLACK;                                               // w 오른쪽 자식을 black으로 변경
                left_rotate(t, x->parent);                                                    // x의 부모를 기준으로 left rotate
                x = t->root;                                                                  // x를 트리의 루트로 인식
            }
        } else {                                                                    
            w = x->parent->left;                                                          // 삭제한 노드의 자식이 삭제한 노드의 부모의 오른쪽 자식일 때, 왼쪽일 때의 반대로 수행
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x ->parent->left;
            }
            if (w->right ->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
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
                w->left -> color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;                                                       
    return;
}


// 직후 원소 찾기, x는 삭제할 노드의 오른쪽 자식
node_t *rbtree_successor(rbtree *t, node_t *x) {
  node_t *y = x;              // y는 x
  while (y->left != t->nil) { // y의 왼쪽 자식이 nil이 아닐 때 반복
    y = y->left;              // y는 y의 왼쪽 자식
  }
  return y;                   // y를 반환
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;                          // y는 삭제할 노드
  color_t p_original_color = y->color;    // p_original_color 는 y의 색깔
  node_t *x;
  
  if (p->left == t->nil) {                // 삭제할 노드의 왼쪽이 nil일때
    x = p->right;                         // x는 삭제할 노드의 오른쪽
    rbtree_transplant(t, p, p->right);    // 삭제할 노드의 부모와 삭제할 노드의 오른쪽 연결 
  } else if (p->right == t->nil) {        // 삭제할 노드의 오른쪽이 nil일때
    x = p->left;                          // x는 삭제할 노드의 왼쪽
    rbtree_transplant(t, p, p->left);     // 삭제할 노드의 부모와 삭제할 노드의 왼쪽을 연결
  } else {
    y = rbtree_successor(t, p->right);    // y는 직후 원소
    p_original_color = y->color;          // p_original_color는 직후 원소의 색
    x = y->right;                         // x는 y의 오른쪽 자식
    if (y->parent == p) {                 // y의 부모가 삭제할 노드일 때
      x->parent = y;                      // x의 부모는 y
    } else {                              // y의 부모가 삭제할 노드가 아닐 때
      rbtree_transplant(t, y, y->right);  // y의 부모와 y의 오른쪽 자식을 연결
      y->right = p->right;                // y의 오른쪽 자식은 삭제할 노드의 오른쪽 자식
      y->right->parent = y;               // y의 오른쪽 자식의 부모는 y
    }
    rbtree_transplant(t, p, y);           // 삭제할 노드 부모와 y를 연결
    y->left = p->left;                    // y의 왼쪽 자식은 삭제할 노드의 왼쪽 자식
    y->left->parent = y;                  // y의 왼쪽 자식의 부모는 y
    y->color = p->color;                  // y의 색은 삭제할 노드의 색
  }
  free(p);                                // 삭제한 노드가 가리키는 공간 삭제
  p = NULL;                               // 할당 해제 후 삭제한 노드값을 NULL로 초기화
  if (p_original_color == RBTREE_BLACK) { // p_original_color가 BLACK 일 때(특성 5 위반)
    rbtree_erase_fixup(t, x);                // 노드의 색을 바꿈
  }
  return 0;
}

// 중위 순회
//  rb트리를 가리키는 *t, 순회할 서브트리의 루트 노드를 가리키는 *root, 정수형 카운터를 가리키는 *cnt, key_t 배열을 가리키는 *arr, 그리고 배열의 크기 n을 인자로 받음
void rbtree_inorder(const rbtree *t, node_t *root, int *cnt, key_t *arr, int n)
{
  if (root->left != t->nil)
    rbtree_inorder(t, root->left, cnt, arr, n);       // 중위순회 방식으로 트리를 순회하며, 왼쪽 서브트리를 먼저 방문
  if (*cnt < n)                                       // 만약 현재 카운터 값이 n보다 작다면, 현재 노드의 키 값을 배열에 저장하고 카운터 값을 증가, 그렇지 않다면, 함수를 종료
  {
    arr[*cnt] = root->key;
    (*cnt)++;
  }
  else
    return;

  if (root->right != t->nil)
    rbtree_inorder(t, root->right, cnt, arr, n);

}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  int counter = 0;
  int *ptr_counter = &counter;
  rbtree_inorder(t, t->root, ptr_counter, arr, n);    // rbtree_inorder 함수를 호출하여 중위순회를 수행하고, 키 값을 오름차순으로 배열에 저장

  return 0;
}