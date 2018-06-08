#include <stdio.h>
#include <stdlib.h>

typedef struct Elem Elem;
typedef struct Node Node;

struct Elem {
  union {
    int int_val;
    char char_val;
  };
};

struct Node {
  int id;
  char *value;
  Elem elem;
};

int pt(int a, char *b, int c) {
  printf("%d %s %d\n", a, b, c);
  return a - 1;
}

void pt1() { printf("end\n"); }

int main() {
  Node node1;

  node1.id = 5;
  node1.value = (char *)malloc(sizeof(char) * 4);
  node1.value[0] = 'a';
  node1.value[1] = 'b';
  node1.value[2] = 'c';
  node1.value[3] = 0;

  node1.elem.int_val = 0;

  int i;
  int array[10];
  for (i = 0; i < 10; ++i) {
    if (i == 0) {
      continue;
    }

    array[i] = i;
    printf("%d\n", array[i]);

    if (i > 4) {
      break;
    }
  }

  if (node1.id < 4) {
    node1.elem.char_val = 1;
    printf("re4 %d\n", pt(node1.id, node1.value, node1.elem.int_val));
  } else if (node1.id < 6) {
    node1.elem.char_val = 2;
    printf("de4 %d\n", pt(node1.id, node1.value, node1.elem.int_val));
  } else {
    node1.elem.char_val = 3;
    printf("de4 %d\n", pt(node1.id, node1.value, node1.elem.int_val));
  }

  while (i < 10) {
    pt1();
    ++i;
  }
}