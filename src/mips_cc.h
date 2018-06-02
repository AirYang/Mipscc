#include <memory>
#include <string>
#include <vector>

#ifndef SRC_MIPS_CC_
#define SRC_MIPS_CC_

class Mipscc {
 public:
  Mipscc(int argc, char** argv);

 public:
  void run();

 private:
  void bufferInit();

 private:
  std::string file_;
  bool need_lexer_;
  std::shared_ptr<std::vector<char>> buffer_;
};

#endif  // SRC_MIPS_CC_