#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "type.h"

#ifndef SRC_TOKEN_
#define SRC_TOKEN_

class Lexer {
 public:
  Lexer(std::shared_ptr<std::vector<char>> buffer);

 public:
  std::shared_ptr<std::vector<Token>> tokenize();

 private:
  void bufferInit(std::shared_ptr<std::vector<char>> buffer);
  void jumpChars(size_t num);
  void jumpUnuseChars();
  Token nextToken();
  bool strStartWith(const std::string& str);
  std::pair<char, size_t> getConstChar(size_t index);

 private:
  size_t cur_;
  size_t col_;
  size_t row_;
  std::shared_ptr<std::vector<char>> buffer_;
};

#endif  // SRC_TOKEN_