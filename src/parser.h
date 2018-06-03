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
  void init();

  void addToFunc(std::shared_ptr<Function> f);
  void addVarToEnv(std::shared_ptr<Identifier> var);
  void addTypeToEnv(std::shared_ptr<PType> type);

  std::shared_ptr<PType> parseTypeSpecifier(std::shared_ptr<Declarator> defs);
  std::shared_ptr<Declarator> parseDeclarators();
  std::shared_ptr<Declarator> parseDeclarator();
  std::shared_ptr<Declarator> parsePlainDeclarator();
  std::shared_ptr<Identifier> parseParameters();

  std::shared_ptr<Identifier> addId(std::shared_ptr<Identifier>& head,
                                    std::shared_ptr<Identifier> id);
  std::shared_ptr<PType> addType(std::shared_ptr<PType>& head,
                                 std::shared_ptr<PType> type);

  std::shared_ptr<Function> findFunc(const std::string& id);
  std::shared_ptr<PType> findType(std::shared_ptr<PType> iter, std::string id);
  std::shared_ptr<Identifier> findId(std::shared_ptr<Identifier> iter,
                                     const std::string& id);

  std::shared_ptr<ReturnType> makeConstReturnType(int x);

 private:
  size_t cur_;
  std::shared_ptr<std::vector<Token>> tokens_;

  bool parsing_;
  bool add_args_;
  int arg_num_;

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
};

#endif  // SRC_PARSER_