#include "mips_cc.h"

int main(int argc, char** argv) {
  Mipscc mcc(argc, argv);
  mcc.run();
  return 0;
}