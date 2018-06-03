#include "parser.h"

#include <cassert>

Parser::Parser(std::shared_ptr<std::vector<Token>> tokens)
    : cur_(0), tokens_(tokens) {
  init();
}

void Parser::parse() {
  parsing_ = true;
  while (cur_ < tokens_->size()) {
    if (!tokens_->at(cur_).literal_.compare("typedef")) {
      std::shared_ptr<Declarator> def = std::make_shared<Declarator>();

    } else {
    }
    cur_func_ = nullptr;
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

std::shared_ptr<PType> Parser::parseTypeSpecifier(
    std::shared_ptr<Declarator> defs) {
  if (tokens_->at(cur_).type_ == Type::IDENTIFIER) {
    std::shared_ptr<Environment> env = cur_env_;
    std::shared_ptr<Identifier> id = nullptr;
    while (env != nullptr) {
      if ((id = findId(env->ids_, tokens_->at(cur_).literal_)) != nullptr) {
        ++cur_;
        assert(id->is_var_ == 0);
        defs->dim_ = id->array_;
        defs->level_ = id->level_;
        return id->type_;
      }
      env = env->pre_;
    }
    assert(false);
  } else if (!tokens_->at(cur_).literal_.compare("int")) {
    ++cur_;
    return type_int_;
  } else if (!tokens_->at(cur_).literal_.compare("char")) {
    ++cur_;
    return type_char_;
  } else if (!tokens_->at(cur_).literal_.compare("void")) {
    ++cur_;
    return type_void_;
  } else if ((!tokens_->at(cur_).literal_.compare("struct")) ||
             (!tokens_->at(cur_).literal_.compare("union"))) {
    int is_struct = !tokens_->at(cur_).literal_.compare("struct");
    std::string id = "";
    ++cur_;
    std::shared_ptr<PType> res = nullptr;
    int same = 1;
    if ((tokens_->at(cur_).type_ == Type::IDENTIFIER) ||
        (!tokens_->at(cur_).literal_.compare("{"))) {
      if (tokens_->at(cur_).type_ == Type::IDENTIFIER) {
        id = tokens_->at(cur_).literal_;
        ++cur_;
        std::shared_ptr<Environment> env = cur_env_;
        while ((env != nullptr) && (res == nullptr)) {
          if ((res = findType(env->types_, id)) != nullptr) {
            break;
          }
          env = env->pre_;
          same = 0;
        }
      }
      if ((res == nullptr) || (!same && res->width_ == -1)) {
        res = std::make_shared<PType>();
        res->literal_ = id;
        res->is_struct_ = is_struct;
        addTypeToEnv(res);
      }
      assert(res->is_struct_ == is_struct);
      if (!tokens_->at(cur_).literal_.compare("{")) {
        assert(res->width_ == -1);
        ++cur_;
        res->width_ = 0;

        while (tokens_->at(cur_).literal_.compare("}")) {
          std::shared_ptr<Declarator> def = std::make_shared<Declarator>();
          std::shared_ptr<PType> sub_type = parseTypeSpecifier(def);
          if (tokens_->at(cur_).literal_.compare(";")) {
          } else {
          }
          assert(!tokens_->at(cur_).literal_.compare(";"));
          ++cur_;
        }
        ++cur_;
        if (res->width_ % 4) {
          res->width_ += 4 - res->width_ % 4;
        }
        return res;
      } else {
        return res;
      }
    }
  } else {
    assert(false);
  }
}

std::shared_ptr<PType> Parser::findType(std::shared_ptr<PType> iter,
                                        std::string id) {
  while (iter != nullptr) {
    if (!iter->literal_.compare(id)) {
      return iter;
    }
    iter = iter->nxt_;
  }
  return nullptr;
}

void Parser::addTypeToEnv(std::shared_ptr<PType> type) {
  assert((!type->literal_.compare("")) ||
         (!findType(cur_env_->types_, type->literal_)));
  assert((addType(cur_env_->types_, type)) != nullptr);
}

std::shared_ptr<PType> Parser::addType(std::shared_ptr<PType>& head,
                                       std::shared_ptr<PType> type) {
  if ((findType(head, type->literal_) == nullptr) ||
      (!type->literal_.compare(""))) {
    type->nxt_ = head;
    head = type;
    return type;
  }
  return nullptr;
}

std::shared_ptr<Declarator> Parser::parseDeclarators() {
  std::shared_ptr<Declarator> res = nullptr;
  std::shared_ptr<Declarator> tmp = nullptr;
  for (res = ;;) {
  }
  return res;
}

std::shared_ptr<Declarator> Parser::parseDeclarator() {}

std::shared_ptr<Declarator> Parser::parsePlainDeclarator() {
  std::shared_ptr<Declarator> res = std::shared_ptr<Declarator>();
  while (!tokens_->at(cur_).literal_.compare("*")) {
    ++cur_;
    ++res->level_;
  }
  assert(tokens_->at(cur_).type_ == Type::IDENTIFIER);
  res->literal_ = tokens_->at(cur_).literal_;
  ++cur_;
  return res;
}

std::shared_ptr<Identifier> Parser::parseParameters() {
  std::shared_ptr<Identifier> res = nullptr;
  if (!tokens_->at(cur_).literal_.compare("(")) {
    int cnt = 0;

    ++cur_;
  }
  return res;
}