#include <stdio.h>
#include <stdlib.h>

struct Node {
  int id;
  char *value;
};

int pt(int a, char *b) {
  printf("%d %s\n", a, b);
  return a - 1;
}

int main() {
  struct Node node1;
  node1.id = 4;
  node1.value = (char *)malloc(sizeof(char) * 4);
  node1.value[0] = 'a';
  node1.value[1] = 'b';
  node1.value[2] = 'c';
  node1.value[3] = 0;

  if (node1.id < 4) {
    printf("re4 %d\n", pt(node1.id, node1.value));
  } else {
    printf("de4 %d\n", pt(node1.id, node1.value));
  }
}