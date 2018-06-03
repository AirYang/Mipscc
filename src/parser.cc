#include "parser.h"

#include <cassert>

Parser::Parser(std::shared_ptr<std::vector<Token>> tokens)
    : cur_(0), tokens_(tokens) {
  init();
}

void Parser::parse() {
  parsing_ = true;
  while (cur_ < tokens_->size()) {
    auto look = tokens_->at(cur_);
    if (!look.literal_.compare("typedef")) {
    } else {
    }
  }
  parsing_ = false;
}

void Parser::init() {
  parsing_ = false;
  add_args_ = false;
  arg_num_ = 0;

  cur_env_ = global_ = std::make_shared<Environment>(nullptr);

  type_int_ = std::make_shared<PType>();
  type_int_->literal_ = "int";
  type_int_->width_ = 4;

  type_char_ = std::make_shared<PType>();
  type_char_->literal_ = "char";
  type_char_->width_ = 1;

  type_void_ = std::make_shared<PType>();
  type_void_->width_ = 1;

  global_->types_ = type_char_;
  type_char_->nxt_ = type_int_;
  type_int_->nxt_ = type_void_;

  std::shared_ptr<Declarator> tmp = std::make_shared<Declarator>();
  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "printf";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "getchar";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "puts";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "putchar";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "exit";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 0;
  tmp->is_func_ = 1;
  tmp->literal_ = "freopen";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 1;
  tmp->is_func_ = 1;
  tmp->literal_ = "malloc";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 1;
  tmp->is_func_ = 1;
  tmp->literal_ = "__print_int__";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 1;
  tmp->is_func_ = 1;
  tmp->literal_ = "__print_string__";
  addToFunc(std::make_shared<Function>(type_void_, nullptr, tmp));

  tmp->level_ = 1;
  tmp->is_func_ = 0;
  tmp->literal_ = "stdin";
  addVarToEnv(std::make_shared<Identifier>(type_void_, 1, nullptr, tmp));

  tmp->dim_ = std::make_shared<Array>();
  tmp->dim_->num_ = 100;
  tmp->literal_ = "__printf_buffer";
  o_buffer_ = std::make_shared<Identifier>(type_int_, 1, nullptr, tmp);
  addVarToEnv(o_buffer_);

  empty_decl_ = std::make_shared<Declarator>();

  const_one_ = makeConstReturnType(1);
  const_zero_ = makeConstReturnType(0);
}

void Parser::addToFunc(std::shared_ptr<Function> f) {
  assert(findFunc(f->id_) == nullptr);
  assert(findId(cur_env_->ids_, f->id_) == nullptr);

  f->nxt_ = func_head_;
  func_head_ = f;
}

std::shared_ptr<Function> Parser::findFunc(const std::string& id) {
  std::shared_ptr<Function> iter = func_head_;
  while (iter != nullptr) {
    if (!iter->id_.compare(id)) {
      return iter;
    }
    iter = iter->nxt_;
  }
  return nullptr;
}

std::shared_ptr<Identifier> Parser::findId(std::shared_ptr<Identifier> iter,
                                           const std::string& id) {
  while ((iter != nullptr) && (iter->id_.compare(id))) {
    iter = iter->nxt_;
  }
  return iter;
}

void Parser::addVarToEnv(std::shared_ptr<Identifier> var) {
  assert(var != nullptr);
  assert(!((var->level_ == 0) && (var->type_ == type_void_)));

  if (cur_env_ == global_) {
    assert(findFunc(var->id_) == nullptr);
  }

  if (add_args_) {
    var->arg_num_ = ++arg_num_;
  }

  var->env_belong_ = cur_env_;
  assert(addId(cur_env_->ids_, var) != nullptr);
}

std::shared_ptr<Identifier> Parser::addId(std::shared_ptr<Identifier>& head,
                                          std::shared_ptr<Identifier> id) {
  if (findId(head, id->id_) == nullptr) {
    id->nxt_ = head;
    head = id;
    return id;
  }
  return nullptr;
}

std::shared_ptr<ReturnType> Parser::makeConstReturnType(int x) {
  std::shared_ptr<ReturnType> res = std::make_shared<ReturnType>();
  res->const_val_ = x;
  res->ret_type_ = CONST_VAL;
  res->ref_ = std::make_shared<Identifier>(type_int_, 0, nullptr,
                                           std::make_shared<Declarator>());
  return res;
}