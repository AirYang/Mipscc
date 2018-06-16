#include <memory>
#include <utility>
#include <vector>

#include "type.h"

#ifndef SRC_PARSER_
#define SRC_PARSER_

class Parser {
 public:
  Parser(std::shared_ptr<std::vector<Token>> tokens);

 public:
  std::pair<std::shared_ptr<Environment>, std::shared_ptr<Function>> parse();
  void showIr();

 private:
  void init();

  void addToFunc(std::shared_ptr<Function> f);
  void addVarToEnv(std::shared_ptr<Identifier> var);
  void addTypeToEnv(std::shared_ptr<PType> type);
  void addVarToType(std::shared_ptr<PType> type,
                    std::shared_ptr<Identifier> var);

  std::shared_ptr<PType> parseTypeSpecifier(std::shared_ptr<Declarator> defs);
  std::shared_ptr<Declarator> parseDeclarators();
  std::shared_ptr<Declarator> parseDeclarator();
  std::shared_ptr<Declarator> parsePlainDeclarator();
  std::shared_ptr<Identifier> parseParameters();
  std::shared_ptr<Array> parseArray();
  std::shared_ptr<ReturnType> parseConstExpr();
  std::shared_ptr<ReturnType> parseLogicOrExpr();
  std::shared_ptr<ReturnType> parseLogicAndExpr();
  std::shared_ptr<ReturnType> parseAndExpr();
  std::shared_ptr<ReturnType> parseXorExpr();
  std::shared_ptr<ReturnType> parseOrExpr();
  std::shared_ptr<ReturnType> parseEqualityExpr();
  std::shared_ptr<ReturnType> parseRelationalExpr();
  std::shared_ptr<ReturnType> parseShiftExpr();
  std::shared_ptr<ReturnType> parseAdditiveExpr();
  std::shared_ptr<ReturnType> parseMultiExpr();
  std::shared_ptr<ReturnType> parseCastExpr();
  std::shared_ptr<ReturnType> parseExpr();
  std::shared_ptr<ReturnType> parseAssignExpr();
  std::shared_ptr<ReturnType> parseUnaryExpr();
  std::shared_ptr<ReturnType> parsePostfix(std::shared_ptr<ReturnType> th);
  std::shared_ptr<ReturnType> parseArguments(std::shared_ptr<Function> func);
  std::shared_ptr<ReturnType> parsePostfixExpr();
  std::shared_ptr<ReturnType> parsePrimaryExpr();
  void parseCompoundStmt(std::shared_ptr<Block> iter_strt,
                         std::shared_ptr<Block> iter_end);
  void parseInitDeclarators(std::shared_ptr<PType> th,
                            std::shared_ptr<Declarator> def);
  void parseInitializer(std::shared_ptr<Identifier> var);
  void parseStmt(std::shared_ptr<Block> iter_strt,
                 std::shared_ptr<Block> iter_end);

  std::shared_ptr<Identifier> addId(std::shared_ptr<Identifier>& head,
                                    std::shared_ptr<Identifier> id);
  std::shared_ptr<PType> addType(std::shared_ptr<PType>& head,
                                 std::shared_ptr<PType> type);

  std::shared_ptr<Function> findFunc(const std::string& id);
  std::shared_ptr<PType> findType(std::shared_ptr<PType> iter, std::string id);
  std::shared_ptr<Identifier> findId(std::shared_ptr<Identifier> iter,
                                     const std::string& id);
  std::shared_ptr<Identifier> findStr(const std::string& s);

  std::shared_ptr<ReturnType> makeConstReturnType(int x);
  std::shared_ptr<ReturnType> makeTmpReturnType();
  std::shared_ptr<ReturnType> makeVarReturnType(
      std::shared_ptr<Identifier> var);
  std::shared_ptr<ReturnType> makeFuncReturnType(
      std::shared_ptr<Function> func);
  std::shared_ptr<ReturnType> deltaMultipler(std::shared_ptr<ReturnType> th);

  std::shared_ptr<ReturnType> mallocInstruction(
      std::shared_ptr<ReturnType> size);
  std::shared_ptr<ReturnType> binaryInstruction(Type op,
                                                std::shared_ptr<ReturnType> l,
                                                std::shared_ptr<ReturnType> r);
  std::shared_ptr<ReturnType> arrayRead(std::shared_ptr<ReturnType> th);
  std::shared_ptr<ReturnType> arrayWrite(std::shared_ptr<ReturnType> l,
                                         std::shared_ptr<ReturnType> r);
  std::shared_ptr<Instruction> insCons(Type ins,
                                       std::shared_ptr<ReturnType> des,
                                       std::shared_ptr<ReturnType> a,
                                       std::shared_ptr<ReturnType> b);
  std::shared_ptr<ReturnType> loadAddress(std::shared_ptr<ReturnType> th);
  void appendIns(std::shared_ptr<Block> th, std::shared_ptr<Instruction> ins);

  bool isString(std::shared_ptr<ReturnType> th);
  bool isInt(std::shared_ptr<ReturnType> th);
  bool isOneDim(std::shared_ptr<ReturnType> th);
  bool isPointer(std::shared_ptr<ReturnType> th);
  bool isType();
  bool isBasicType(std::shared_ptr<PType> th);
  bool canPass(std::shared_ptr<ReturnType> a, std::shared_ptr<ReturnType> b);
  bool canAssign(std::shared_ptr<ReturnType> l, std::shared_ptr<ReturnType> r);
  bool canMul(std::shared_ptr<ReturnType> l, std::shared_ptr<ReturnType> r);
  bool canSub(std::shared_ptr<ReturnType> l, std::shared_ptr<ReturnType> r);

  int varWidth(std::shared_ptr<Identifier> var);

  //   void showReturnType(std::shared_ptr<ReturnType> th);

 private:
  size_t cur_;
  std::shared_ptr<std::vector<Token>> tokens_;

  bool parsing_;
  bool add_args_;
  int arg_num_;
  int str_const_cnt_;

  std::shared_ptr<Environment> cur_env_;
  std::shared_ptr<Environment> global_;

  std::shared_ptr<PType> type_int_;
  std::shared_ptr<PType> type_char_;
  std::shared_ptr<PType> type_void_;

  std::shared_ptr<Function> func_head_;
  std::shared_ptr<Function> cur_func_;

  std::shared_ptr<Identifier> o_buffer_;
  std::shared_ptr<Declarator> empty_decl_;

  std::shared_ptr<ReturnType> const_one_;
  std::shared_ptr<ReturnType> const_zero_;
  std::shared_ptr<ReturnType> printer_;

  std::vector<std::shared_ptr<Instruction>> ins_buffer_;

  std::shared_ptr<Block> block_top_;
  std::shared_ptr<Block> global_init_;
};

#endif  // SRC_PARSER_