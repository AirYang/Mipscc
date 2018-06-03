#include <memory>
#include <vector>

#include "type.h"

#ifndef SRC_PARSER_
#define SRC_PARSER_

class Parser {
 public:
  Parser(std::shared_ptr<std::vector<Token>> tokens);

 public:
  void parse();

 private:
  size_t cur_;
  std::shared_ptr<std::vector<Token>> tokens_;
};

#endif  // SRC_PARSER_