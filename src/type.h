#include <memory>
#include <string>
#include <vector>

#ifndef SRC_TYPE_
#define SRC_TYPE_

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
  INS_BEQ,
  NOT_A_TYPE
};

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

const char* TypeToStr(Type tp);
bool IsBranch(Type tp);
bool IsUnary(Type tp);
bool IsBinary(Type tp);
bool IsBtf(Type tp);
bool IsBb(Type tp);

class Token;
class Function;
class Array;
class Identifier;
class PType;
class Environment;
class Instruction;
class ReturnType;
class Declarator;
class Block;
class InitPair;
class Operand;
class Action;
class Register;

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

class Block {
 public:
  Block();

 public:
  int id_;
  int in_deg_;
  // int ins_size_;
  // int buffer_size_;
  std::vector<Instruction> ins_;
  std::shared_ptr<Block> non_condi_;
  std::shared_ptr<Block> condi_;

 public:
  static int blck_cnt_;
};

class Function {
 public:
  Function(std::shared_ptr<PType> type, std::shared_ptr<Declarator> a,
           std::shared_ptr<Declarator> b);

 public:
  std::shared_ptr<PType> type_;
  int level_;
  int tmp_cnt_;
  std::string id_;
  std::shared_ptr<Identifier> args_;
  std::shared_ptr<Function> nxt_;
  std::shared_ptr<Block> block_;
  std::shared_ptr<Block> end_;
  std::shared_ptr<ReturnType> regs_;
};

class Array {
 public:
  Array();

 public:
  int num_;
  int mul_;
  std::shared_ptr<Array> nxt_;
  std::shared_ptr<Array> pre_;
};

enum { INIT_NONE = 1, INIT_STR, INIT_LIST };

class Identifier {
 public:
  Identifier(std::shared_ptr<PType> type, int is_var,
             std::shared_ptr<Declarator> a, std::shared_ptr<Declarator> b);

  Identifier();

  static std::shared_ptr<Identifier> cloneIdentifier(
      std::shared_ptr<Identifier> id);

 public:
  std::string id_;
  int level_;
  int from_;
  int is_var_;
  int arg_num_;
  std::shared_ptr<Array> array_;
  std::shared_ptr<PType> type_;
  std::shared_ptr<Identifier> nxt_;
  int init_type_;
  std::string init_str_;
  std::string str_val_;
  std::shared_ptr<InitPair> init_list_;
  std::shared_ptr<Environment> env_belong_;
  std::shared_ptr<PType> type_belong_;
};

class PType {
 public:
  PType();

 public:
  int is_struct_;
  int width_;
  std::string literal_;
  std::shared_ptr<PType> nxt_;
  std::shared_ptr<Identifier> mem_;
};

class Environment {
 public:
  Environment(std::shared_ptr<Environment> pre);

 public:
  std::shared_ptr<Environment> pre_;
  std::shared_ptr<PType> types_;
  std::shared_ptr<Identifier> ids_;
};

class Instruction {
 public:
  Instruction();

 public:
  Type ins_;
  int ord_;
  std::shared_ptr<ReturnType> des_;
  std::shared_ptr<ReturnType> a_;
  std::shared_ptr<ReturnType> b_;
};

enum { VIRTUAL_REG = 1, ARRAY_ACCESS, CONST_VAL };

class ReturnType {
 public:
  ReturnType();

  static std::string toString(std::shared_ptr<ReturnType> res);

 public:
  int ret_type_;
  int is_left_;
  int const_val_;
  std::shared_ptr<Function> func_;
  std::shared_ptr<ReturnType> nxt_;
  std::shared_ptr<Identifier> ref_;
  int reg_num_;
  int sp_offset_;
  std::shared_ptr<Function> belong_;
  std::shared_ptr<Register> reg_;
};

class Declarator {
 public:
  Declarator();

 public:
  static std::shared_ptr<Declarator> mergeDecl(std::shared_ptr<Declarator> a,
                                               std::shared_ptr<Declarator> b);

 public:
  std::string literal_;
  int level_;
  std::shared_ptr<Declarator> nxt_;
  int is_func_;
  std::shared_ptr<Identifier> args_;
  std::shared_ptr<Array> dim_;
};

class InitPair {
 public:
  InitPair(int idx, int num, const std::string& label);

 public:
  int pos_;
  int num_;
  std::string label_;
  std::shared_ptr<InitPair> nxt_;
};

class Register {
 public:
  std::string literal_;
  std::shared_ptr<ReturnType> ref_;
};

#endif  // SRC_TYPE_