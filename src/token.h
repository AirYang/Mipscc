#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifndef SRC_TOKEN_
#define SRC_TOKEN_

enum class Type {
  // five kind
  KEY = 0,
  IDENTIFIER,
  INT_CONST,
  CHAR_CONST,
  STRING_CONST,
  OPERATION,
  // operation
  INS_ADD,
  INS_SUB,
  INS_MUL,
  INS_DIV,
  INS_OR,
  INS_XOR,
  INS_AND,
  INS_SLLV,
  INS_SRLV,
  INS_SNE,
  INS_SEQ,
  INS_SGT,
  INS_SLT,
  INS_SGE,
  INS_SLE,
  INS_REM,
  INS_NEG,
  INS_BNEZ,
  INS_BEQZ,
  INS_PARA,
  INS_CALL,
  INS_PRINT_INT,
  INS_PRINT_STRING,
  INS_MALLOC,
  INS_GETCHAR,
  INS_PUTCHAR,
  INS_MOVE,
  INS_NOT,
  INS_LD_ADDR,
  INS_RET,
  INS_HALT,
  INS_ARRAY_WRITE,
  INS_ARRAY_READ,
  INS_BLE,
  INS_BGE,
  INS_BLT,
  INS_BGT,
  INS_BNE,
  INS_BEQ
};

class Token {
 public:
  // Token type five kind
  Type type_;
  // location
  size_t col_;
  size_t row_;
  // literal value
  std::string literal_;
  // if is int
  int int_val_;
  // if is char
  char char_val_;
  // if is str
  std::string str_val_;
};

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