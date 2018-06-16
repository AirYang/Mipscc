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
  void writeOut(std::shared_ptr<std::vector<std::string>> out_buffer);

 private:
  std::string src_file_;
  std::string out_file_;
  bool need_lexer_;
  bool need_parser_;
  bool need_assembler_;
  std::shared_ptr<std::vector<char>> buffer_;
};

#endif  // SRC_MIPS_CC_