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
    for (++cur_; tokens_->at(cur_).literal_.compare(")");) {
      std::shared_ptr<Declarator> def = std::make_shared<Declarator>();
      std::shared_ptr<Declarator> decl = std::make_shared<Declarator>();
      std::shared_ptr<PType> type = parseTypeSpecifier(def);

      if ((!tokens_->at(cur_).literal_.compare("*")) ||
          (tokens_->at(cur_).type_ == Type::IDENTIFIER)) {
        while (!tokens_->at(cur_).literal_.compare("*")) {
          ++cur_;
          ++decl->level_;
        }

        if (tokens_->at(cur_).type_ == Type::IDENTIFIER) {
          decl->literal_ = tokens_->at(cur_).literal_;
          ++cur_;
          decl->dim_ =
        }
      }

      std::shared_ptr<Identifier> tmp =
          std::make_shared<Identifier>(type, 1, def, decl);
      if ((tmp->type_ == type_void_) && (tmp->level_ == 0)) {
      }

      tmp->nxt_ = res;
      res = tmp;
      if (!tokens_->at(cur_).literal_.compare(",")) {
      }
    }
    ++cur_;
  }
  return res;
}

std::shared_ptr<Array> Parser::parseArray() {
  std::shared_ptr<Array> res = nullptr;
  if (!tokens_->at(cur_).literal_.compare("[")) {
    std::shared_ptr<Array> tmp = nullptr;
    std::shared_ptr<Array> nxt = nullptr;
    while (!tokens_->at(cur_).literal_.compare("[")) {
      tmp = std::shared_ptr<Array>();
      ++cur_;
      tmp->num_ = -1;
      if (tokens_->at(cur_).literal_.compare("]")) {
        std::shared_ptr<ReturnType> num = parse
      } else {
      }
      assert(!tokens_->at(cur_).literal_.compare("]"));
      ++cur_;
    }
    nxt = nullptr;
    tmp = nullptr;
    int mul = 1;
    while (res != nullptr) {
    }
    res = tmp;
    while (tmp->nxt_ != nullptr) {
    }
  }
  return res;
}

std::shared_ptr<ReturnType> Parser::parseConstExpr(){
    std::shared_ptr<ReturnType> res}

std::shared_ptr<ReturnType> Parser::parseLogicOrExpr() {}

std::shared_ptr<ReturnType> Parser::parseLogicAndExpr() {}

std::shared_ptr<ReturnType> Parser::parseAndExpr() {}

std::shared_ptr<ReturnType> Parser::parseXorExpr() {}

std::shared_ptr<ReturnType> Parser::parseOrExpr() {}

std::shared_ptr<ReturnType> Parser::parseEqualityExpr() {}

std::shared_ptr<ReturnType> Parser::parseRelationalExpr() {}

std::shared_ptr<ReturnType> Parser::parseShiftExpr() {}

std::shared_ptr<ReturnType> Parser::parseAdditiveExpr() {}

std::shared_ptr<ReturnType> Parser::parseMultiExpr() {}

std::shared_ptr<ReturnType> Parser::parseCastExpr() {}

std::shared_ptr<ReturnType> Parser::parseExpr() {
  std::shared_ptr<ReturnType> res = parseAssignExpr();
  while (!tokens_->at(cur_).literal_.compare(",")) {
    ++cur_;
    res = parseAssignExpr();
  }
  return res;
}

std::shared_ptr<ReturnType> Parser::parseAssignExpr() {
  std::shared_ptr<ReturnType> l = parseLogicOrExpr();
  std::string op = "";
  if ((tokens_->at(cur_).literal_[0] == '=') ||
      (tokens_->at(cur_).literal_[1] == '=') ||
      (tokens_->at(cur_).literal_[2] == '=')) {
    assert(l->is_left_);
    op = tokens_->at(cur_).literal_;
    std::shared_ptr<ReturnType> r = parseAssignExpr();
    std::shared_ptr<ReturnType> res = nullptr;

    if (!op.compare("+=")) {
      res = binaryInstruction(Type::INS_ADD, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("-=")) {
      res = binaryInstruction(Type::INS_SUB, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("*=")) {
      res = binaryInstruction(Type::INS_MUL, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("/=")) {
      res = binaryInstruction(Type::INS_DIV, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("%=")) {
      res = binaryInstruction(Type::INS_REM, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("<<=")) {
      res = binaryInstruction(Type::INS_SLLV, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare(">>=")) {
      res = binaryInstruction(Type::INS_SLLV, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("&=")) {
      res = binaryInstruction(Type::INS_AND, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("^=")) {
      res = binaryInstruction(Type::INS_XOR, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else if (!op.compare("|=")) {
      res = binaryInstruction(Type::INS_OR, l, r);
      res->ref_ = Identifier::cloneIdentifier(l->ref_);
    } else {
      res = r;
    }

    if (l->ret_type_ == ARRAY_ACCESS) {
      arrayWrite(l, res);
    } else {
      if (res->ret_type_ == ARRAY_ACCESS) {
        res = arrayRead(res);
      }
      appendIns(block_top_, insCons(Type::INS_MOVE, l, res, nullptr));
    }
    l = res;
  }
  return l;
}

std::shared_ptr<ReturnType> Parser::binaryInstruction(
    Type op, std::shared_ptr<ReturnType> l, std::shared_ptr<ReturnType> r) {
  if (l->const_val_ && r->const_val_) {
    if (op == Type::INS_ADD) {
      return makeConstReturnType(l->const_val_ + r->const_val_);
    } else if (op == Type::INS_SUB) {
      return makeConstReturnType(l->const_val_ - r->const_val_);
    } else if (op == Type::INS_MUL) {
      return makeConstReturnType(l->const_val_ * r->const_val_);
    } else if (op == Type::INS_DIV) {
      return makeConstReturnType(l->const_val_ / r->const_val_);
    } else if (op == Type::INS_REM) {
      return makeConstReturnType(l->const_val_ % r->const_val_);
    }
  }

  if (l->ret_type_ == ARRAY_ACCESS) {
    l = arrayRead(l);
  }

  if (r->ret_type_ == ARRAY_ACCESS) {
    r = arrayRead(r);
  }

  std::shared_ptr<ReturnType> res = makeTmpReturnType();
  appendIns(block_top_, insCons(op, res, l, r));
  res->ref_ = std::make_shared<Identifier>(nullptr, 0, nullptr, empty_decl_);
  return res;
}

std::shared_ptr<ReturnType> Parser::arrayRead(std::shared_ptr<ReturnType> th) {
  assert(th->ret_type_ == ARRAY_ACCESS);
  std::shared_ptr<ReturnType> res = makeTmpReturnType();
  res->ref_ = Identifier::cloneIdentifier(th->ref_);
  appendIns(block_top_, insCons(Type::INS_ARRAY_READ, res, th, nullptr));
  return res;
}

std::shared_ptr<ReturnType> Parser::makeTmpReturnType() {
  std::shared_ptr<ReturnType> res = std::make_shared<ReturnType>();
  res->ret_type_ = VIRTUAL_REG;
  res->belong_ = cur_func_;
  res->reg_num_ = ++cur_func_->tmp_cnt_;
  res->nxt_ = cur_func_->regs_;
  res->sp_offset_ = 0;
  cur_func_->regs_ = res;
  return res;
}

std::shared_ptr<Instruction> Parser::insCons(Type ins,
                                             std::shared_ptr<ReturnType> des,
                                             std::shared_ptr<ReturnType> a,
                                             std::shared_ptr<ReturnType> b) {
  std::shared_ptr<Instruction> res = std::make_shared<Instruction>();
  res->ins_ = ins;
  res->des_ = des;
  res->a_ = a;
  res->b_ = b;
  ins_buffer_.push_back(res);
  res->ord_ = ins_buffer_.size();
  return res;
}

void Parser::appendIns(std::shared_ptr<Block> th,
                       std::shared_ptr<Instruction> ins) {
  if ((cur_env_ == global_) && parsing_) {
    th = global_init_;
  }

  assert(th != nullptr);
  assert(ins != nullptr);

  if ((ins->ins_ == Type::INS_MOVE) && (ins->des_->ref_ != nullptr) &&
      (!ins->des_->ref_->id_.compare("")) && (ins->des_->reg_num_ == -1) &&
      (ins->des_->ref_->arg_num_ == -1)) {
    assert(false);
  }

  th->ins_.push_back(*ins);
}

std::shared_ptr<ReturnType> Parser::arrayWrite(std::shared_ptr<ReturnType> l,
                                               std::shared_ptr<ReturnType> r) {
  if (r->ret_type_ == ARRAY_ACCESS) {
    r = arrayRead(r);
  }
  appendIns(block_top_, insCons(Type::INS_ARRAY_WRITE, nullptr, l, r));
  return r;
}

std::shared_ptr<ReturnType> Parser::parseUnaryExpr() {
  if ((!tokens_->at(cur_).literal_.compare("++")) ||
      (!tokens_->at(cur_).literal_.compare("--"))) {
    Type op = Type::NOT_A_TYPE;
    if (!tokens_->at(cur_).literal_.compare("++")) {
      op = Type::INS_ADD;
    } else {
      op = Type::INS_SUB;
    }
    ++cur_;
    std::shared_ptr<ReturnType> th = parseUnaryExpr();
    std::shared_ptr<ReturnType> res = nullptr;
    assert(th->is_left_);
    if (th->ret_type_ == ARRAY_ACCESS) {
      res = binaryInstruction(op, arrayRead(th), const_one_);
      res->ref_ = Identifier::cloneIdentifier(th->ref_);
      arrayWrite(th, res);
    } else {
      if (isPointer(th)) {
        res = binaryInstruction(op, th, deltaMultipler(th));
        res->ref_ = Identifier::cloneIdentifier(th->ref_);
        appendIns(block_top_, insCons(Type::INS_MOVE, th, res, nullptr));
      } else {
        appendIns(block_top_, insCons(op, th, th, const_one_));
      }
    }
    return res;
  } else if (!tokens_->at(cur_).literal_.compare("sizeof")) {
    ++cur_;
    if (!tokens_->at(cur_).literal_.compare("(")) {
      ++cur_;
      if (isType()) {
        std::shared_ptr<Declarator> def = std::make_shared<Declarator>();
        std::shared_ptr<PType> type = parseTypeSpecifier(def);
        bool is_pointer = (def != nullptr) && def->level_;
        int res = 0;
        while (!tokens_->at(cur_).literal_.compare("*")) {
          ++cur_;
          is_pointer = true;
        }
        assert(!tokens_->at(cur_).literal_.compare(")"));
        ++cur_;
        if (is_pointer) {
          res = 4;
        } else {
          assert(type->width_ != -1);
          res = type->width_;
        }
        if ((def != nullptr) && (def->dim_ != nullptr)) {
          res *= def->dim_->num_ * def->dim_->mul_;
        }
        return makeConstReturnType(res);
      } else {
        std::shared_ptr<ReturnType> res = parseExpr();
        assert(tokens_->at(cur_).literal_.compare(")"));
        ++cur_;
        return res;
      }
    } else {
      std::shared_ptr<ReturnType> th = parseUnaryExpr();
      int res = 0;
      if (th->ref_->level_) {
        res = 4;
      } else {
        assert(th->ref_->type_->width_ != -1);
        res = th->ref_->type_->width_;
      }
      return makeConstReturnType(res);
    }
  } else if (!tokens_->at(cur_).literal_.compare("&")) {
    ++cur_;
    std::shared_ptr<ReturnType> th = parseCastExpr();
    assert(th->is_left_);
    return loadAddress(th);
  } else if (!tokens_->at(cur_).literal_.compare("*")) {
    ++cur_;
    std::shared_ptr<ReturnType> th = parseCastExpr();
    std::shared_ptr<ReturnType> res = makeTmpReturnType();
    if (th->ret_type_ == ARRAY_ACCESS) {
      th = arrayRead(th);
    }
    assert(th->ref_->level_ || (th->ref_->array_ != nullptr));
    res->ref_ = Identifier::cloneIdentifier(th->ref_);
    appendIns(block_top_, insCons(Type::INS_MOVE, res, th, nullptr));
    if (res->ref_->array_ != nullptr) {
      res->ref_->array_ = res->ref_->array_->nxt_;
      if (res->ref_->array_ == nullptr) {
        res->is_left_ = 1;
      }

      if (isInt(res) || res->ref_->level_) {
        res->ret_type_ = ARRAY_ACCESS;
      }
    } else {
      res->is_left_ = 1;
      if ((--res->ref_->level_) || (isInt(res))) {
        res->ret_type_ = ARRAY_ACCESS;
      } else {
        res->ret_type_ = VIRTUAL_REG;
      }
    }
    return res;
  } else if (!tokens_->at(cur_).literal_.compare("+")) {
  } else if (!tokens_->at(cur_).literal_.compare("-")) {
  } else if (!tokens_->at(cur_).literal_.compare("~")) {
  } else if (!tokens_->at(cur_).literal_.compare("!")) {
  } else {
  }

  return nullptr;
}

std::shared_ptr<ReturnType> Parser::parsePostfix(
    std::shared_ptr<ReturnType> th) {
  while (true) {
    if (!tokens_->at(cur_).literal_.compare("(")) {
      assert(th->func_ != nullptr);
      ++cur_;
      th = parseArguments(th->func_);
      assert(!tokens_->at(cur_).literal_.compare(")"));
      ++cur_;
    } else if (!tokens_->at(cur_).literal_.compare("[")) {
      assert((th->ref_->level_) || (th->ref_->array_ != nullptr));
      ++cur_;
      std::shared_ptr<ReturnType> idx = parseExpr();
      assert(!idx->ref_->level_);
      assert((idx->ref_->type_ == type_int_) ||
             (idx->ref_->type_ == type_char_));
      if (idx->ret_type_ == ARRAY_ACCESS) {
        idx = arrayRead(idx);
      }
      if (idx->ret_type_ == CONST_VAL) {
        idx = makeConstReturnType(idx->const_val_ *
                                  deltaMultipler(th)->const_val_);
      } else {
        if (deltaMultipler(th)->const_val_ != -1) {
          idx = binaryInstruction(Type::INS_MUL, idx, deltaMultipler(th));
          idx->ref_ = Identifier::cloneIdentifier(const_one_->ref_);
        }
      }
      if (th->ret_type_ == ARRAY_ACCESS) {
        if (th->ref_->level_) {
          th = arrayRead(th);
        } else {
          th->ret_type_ = VIRTUAL_REG;
        }
      }
      std::shared_ptr<ReturnType> res =
          binaryInstruction(Type::INS_ADD, th, idx);
      res->ref_ = Identifier::cloneIdentifier(th->ref_);
      if (res->ref_->array_ != nullptr) {
        res->ref_->array_ = res->ref_->array_->nxt_;
        if (res->ref_->array_ == nullptr) {
          res->is_left_ = 1;
        }
        if (isInt(res) || res->ref_->level_) {
          res->ret_type_ = ARRAY_ACCESS;
        }
      } else if (res->ref_->level_) {
        res->is_left_ = 1;
        if (--res->ref_->level_ || isInt(res)) {
          res->ret_type_ = ARRAY_ACCESS;
        }
      }
      assert(!tokens_->at(cur_).literal_.compare("]"));
      ++cur_;
      th = res;
    } else if (!tokens_->at(cur_).literal_.compare(".")) {
      ++cur_;
      assert((!th->ref_->level_) && (th->ref_->array_ == nullptr));
      assert(tokens_->at(cur_).type_ == Type::IDENTIFIER);
      std::shared_ptr<Identifier> mem = th->ref_->type_->mem_;
      while ((mem != nullptr) &&
             (mem->id_.compare(tokens_->at(cur_).literal_))) {
        mem = mem->nxt_;
      }
      assert(mem != nullptr);
      std::shared_ptr<ReturnType> res =
          binaryInstruction(Type::INS_ADD, th, makeConstReturnType(mem->from_));
      res->ref_ = Identifier::cloneIdentifier(mem);
      res->is_left_ = (mem->array_ == nullptr) ? 1 : 0;
      if ((mem->array_ == nullptr) && (isInt(res) || res->ref_->level_)) {
        res->ret_type_ = ARRAY_ACCESS;
      }
      ++cur_;
      th = res;
    } else if (!tokens_->at(cur_).literal_.compare("->")) {
      ++cur_;
      assert((th->ref_->level_ == -1) || ((th->ref_->array_ != nullptr) &&
                                          (th->ref_->array_->nxt_ == nullptr)));
      assert(tokens_->at(cur_).type_ == Type::IDENTIFIER);
      std::shared_ptr<Identifier> mem = th->ref_->type_->mem_;
      while ((mem != nullptr) &&
             (mem->id_.compare(tokens_->at(cur_).literal_))) {
        mem = mem->nxt_;
      }
      assert(mem != nullptr);
      if (th->ret_type_ == ARRAY_ACCESS) {
        th = arrayRead(th);
      }
      std::shared_ptr<ReturnType> res =
          binaryInstruction(Type::INS_ADD, th, makeConstReturnType(mem->from_));
      res->ref_ = Identifier::cloneIdentifier(mem);
      res->is_left_ = (mem->array_ == nullptr) ? 1 : 0;
      if ((res->ref_->array_ == nullptr) &&
          ((isInt(res)) || (res->ref_->level_))) {
        res->ret_type_ = ARRAY_ACCESS;
      }
      ++cur_;
      th = res;
    } else if ((!tokens_->at(cur_).literal_.compare("++")) ||
               (!tokens_->at(cur_).literal_.compare("--"))) {
      Type op = Type::NOT_A_TYPE;
      if (!tokens_->at(cur_).literal_.compare("++")) {
        op = Type::INS_ADD;
      } else {
        op = Type::INS_SUB;
      }
      ++cur_;
      assert(((th->ref_->type_ == type_char_) ||
              (th->ref_->type_ == type_int_) || (th->ref_->level_)) &&
             (th->ref_->array_ == nullptr) && (th->is_left_));
      std::shared_ptr<ReturnType> val = nullptr;
      std::shared_ptr<ReturnType> res = nullptr;
      if (th->ret_type_ == ARRAY_ACCESS) {
        res = arrayRead(th);
        val = binaryInstruction(op, res, const_one_);
        val->ref_ = Identifier::cloneIdentifier(th->ref_);
        arrayWrite(th, val);
      } else {
        val = binaryInstruction(op, th, deltaMultipler(th));
        val->ref_ = Identifier::cloneIdentifier(th->ref_);
        res = makeTmpReturnType();
        appendIns(block_top_, insCons(Type::INS_MOVE, res, th, nullptr));
        appendIns(block_top_, insCons(Type::INS_MOVE, th, val, nullptr));
        res->ref_ = Identifier::cloneIdentifier(th->ref_);
      }
      th = res;
    } else {
      return th;
    }
  }
}

std::shared_ptr<ReturnType> Parser::parseArguments(
    std::shared_ptr<Function> func) {
  if (!func->id_.compare("__print_int__")) {
    std::shared_ptr<ReturnType> res = parseAssignExpr();
    if (res->ret_type_ == ARRAY_ACCESS) {
      res = arrayRead(res);
    }
    appendIns(block_top_, insCons(Type::INS_PRINT_INT, nullptr, res, nullptr));
    return nullptr;
  } else if (!func->id_.compare("__print_string__")) {
    std::shared_ptr<ReturnType> res = parseAssignExpr();
    if (res->ret_type_ == ARRAY_ACCESS) {
      res = arrayRead(res);
    }
    appendIns(block_top_,
              insCons(Type::INS_PRINT_STRING, nullptr, res, nullptr));
    return nullptr;
  } else if (!func->id_.compare("printf")) {
    std::vector<std::shared_ptr<ReturnType>> nd_buffer;
    std::shared_ptr<ReturnType> format = parseAssignExpr();
    assert(isString(format));
    // int arg_cnt = 0;
    // int i = 0;
    if (!tokens_->at(cur_).literal_.compare(",")) {
      ++cur_;
    }

    while (tokens_->at(cur_).literal_.compare(")")) {
      std::shared_ptr<ReturnType> tmp = parseAssignExpr();
      if (tmp->ret_type_ == ARRAY_ACCESS) {
        tmp = arrayRead(tmp);
      }

      nd_buffer.push_back(tmp);

      if (!tokens_->at(cur_).literal_.compare(",")) {
        ++cur_;
      }
    }

    if (nd_buffer.size()) {
      if (format->ref_->id_[0] == '$') {
        std::shared_ptr<Identifier> id = findStr(format->ref_->id_);
        if (!id->str_val_.compare("%d")) {
          appendIns(block_top_, insCons(Type::INS_PRINT_INT, nullptr,
                                        nd_buffer[0], nullptr));
          return nullptr;
        }

        if (!id->str_val_.compare("%d ")) {
          appendIns(block_top_, insCons(Type::INS_PRINT_INT, nullptr,
                                        nd_buffer[0], nullptr));

          appendIns(block_top_, insCons(Type::INS_PUTCHAR, nullptr,
                                        makeConstReturnType(' '), nullptr));
          return nullptr;
        }

        if (!id->str_val_.compare("%d\n")) {
          appendIns(block_top_, insCons(Type::INS_PRINT_INT, nullptr,
                                        nd_buffer[0], nullptr));

          appendIns(block_top_, insCons(Type::INS_PUTCHAR, nullptr,
                                        makeConstReturnType('\N'), nullptr));
          return nullptr;
        }

        if (!id->str_val_.compare("%c")) {
          appendIns(block_top_,
                    insCons(Type::INS_PUTCHAR, nullptr, nd_buffer[0], nullptr));
          return nullptr;
        }
      }

      std::shared_ptr<ReturnType> para_buffer = makeVarReturnType(o_buffer_);
      for (size_t i = 0; i < nd_buffer.size(); ++i) {
        std::shared_ptr<ReturnType> addr = binaryInstruction(
            Type::INS_ADD, para_buffer, makeConstReturnType(i * 4));
        addr->ret_type_ = ARRAY_ACCESS;
        arrayWrite(addr, nd_buffer[i]);
      }
      appendIns(block_top_,
                insCons(Type::INS_PARA, nullptr, para_buffer, const_one_));
      appendIns(block_top_,
                insCons(Type::INS_PARA, nullptr, format, const_one_));
      appendIns(block_top_,
                insCons(Type::INS_CALL, nullptr, printer_, nullptr));
    } else {
      appendIns(block_top_,
                insCons(Type::INS_PRINT_STRING, nullptr, format, nullptr));
    }
  } else if (!func->id_.compare("getchar")) {
    assert(!tokens_->at(cur_).literal_.compare(")"));
    std::shared_ptr<ReturnType> res = makeTmpReturnType();
    res->ref_ =
        std::make_shared<Identifier>(type_int_, 0, nullptr, empty_decl_);
    appendIns(block_top_, insCons(Type::INS_GETCHAR, res, nullptr, nullptr));
    return res;
  } else if (!func->id_.compare("putchar")) {
    std::shared_ptr<ReturnType> tmp = parseAssignExpr();
    assert(!tokens_->at(cur_).literal_.compare(")"));
    if (tmp->ret_type_ == ARRAY_ACCESS) {
      tmp = arrayRead(tmp);
    }
    appendIns(block_top_, insCons(Type::INS_PUTCHAR, nullptr, tmp, nullptr));
    return nullptr;
  } else if (!func->id_.compare("exit")) {
    std::shared_ptr<ReturnType> tmp = parseAssignExpr();
    assert(isInt(tmp));
    if (tmp->ret_type_ == ARRAY_ACCESS) {
      tmp = arrayRead(tmp);
    }
    appendIns(block_top_, insCons(Type::INS_HALT, nullptr, tmp, nullptr));
    assert(!tokens_->at(cur_).literal_.compare(")"));
    return nullptr;
  } else if (!func->id_.compare("malloc")) {
    std::shared_ptr<ReturnType> tmp = parseAssignExpr();
    if (tmp->ret_type_ == ARRAY_ACCESS) {
      tmp = arrayRead(tmp);
    }
    assert(isInt(tmp));
    assert(!tokens_->at(cur_).literal_.compare(")"));
    return mallocInstruction(tmp);
  } else {
    std::vector<std::shared_ptr<ReturnType>> nd_buffer;
    while (tokens_->at(cur_).literal_.compare(")")) {
      std::shared_ptr<ReturnType> tmp = parseAssignExpr();
      nd_buffer.push_back(tmp);
      if (!tokens_->at(cur_).literal_.compare(",")) {
        ++cur_;
      }
    }
    std::shared_ptr<ReturnType> l = std::make_shared<ReturnType>();
    std::shared_ptr<Identifier> arg = nullptr;
    int i = 0;
    for (arg = func->args_, i = (int)(nd_buffer.size()) - 1;
         (arg != nullptr) && (i >= 0); arg = arg->nxt_, --i) {
      l->ref_ = Identifier::cloneIdentifier(arg);
      assert(canPass(l, nd_buffer[i]));
      if (nd_buffer[i]->ret_type_ == ARRAY_ACCESS) {
        nd_buffer[i] = arrayRead(nd_buffer[i]);
      }
      std::shared_ptr<ReturnType> bi = std::shared_ptr<ReturnType>();
      bi->ref_ = arg;
      appendIns(block_top_, insCons(Type::INS_PARA, nullptr, nd_buffer[i], bi));
    }
    assert((arg == nullptr) && (i < 0));
    std::shared_ptr<ReturnType> res = makeTmpReturnType();
    std::shared_ptr<Declarator> decl = std::make_shared<Declarator>();
    decl->level_ = func->level_;
    res->ref_ = std::make_shared<Identifier>(func->type_, 0, nullptr, decl);
    appendIns(block_top_,
              insCons(Type::INS_CALL, res, makeFuncReturnType(func), nullptr));
    return res;
  }
  return nullptr;
}

bool Parser::isString(std::shared_ptr<ReturnType> th) {
  if ((th->ref_->array_ != nullptr) && (th->ref_->array_->nxt_ == nullptr) &&
      (th->ref_->type_ == type_char_)) {
    return true;
  }

  if ((th->ref_->type_ == type_char_) && (th->ref_->level_ == 1)) {
    return true;
  }

  return false;
}

std::shared_ptr<ReturnType> Parser::makeVarReturnType(
    std::shared_ptr<Identifier> var) {
  if (cur_func_ != nullptr) {
    std::shared_ptr<ReturnType> iter = cur_func_->regs_;
    while ((iter != nullptr) && (iter->ref_ != var)) {
      iter = iter->nxt_;
    }
    if (iter) {
      return iter;
    }
  }

  assert(var->is_var_);

  std::shared_ptr<ReturnType> res = std::make_shared<ReturnType>();
  res->ret_type_ = VIRTUAL_REG;
  res->ref_ = var;
  res->belong_ = cur_func_;
  res->is_left_ = (var->array_ == nullptr) ? 1 : 0;
  if (var->env_belong_ != global_) {
    res->nxt_ = cur_func_->regs_;
    cur_func_->regs_ = res;
  }
  return res;
}

std::shared_ptr<Identifier> Parser::findStr(const std::string& s) {
  std::shared_ptr<Identifier> iter = global_->ids_;
  while ((iter != nullptr) && (iter->id_.compare(s))) {
    iter = iter->nxt_;
  }
  return iter;
}

bool Parser::isInt(std::shared_ptr<ReturnType> th) {
  return ((th->ref_->type_ == type_char_) || (th->ref_->type_ == type_int_)) &&
         (th->ref_->array_ == nullptr) && (!th->ref_->level_);
}

std::shared_ptr<ReturnType> Parser::mallocInstruction(
    std::shared_ptr<ReturnType> size) {
  std::shared_ptr<ReturnType> res = makeTmpReturnType();
  res->ref_ = std::make_shared<Identifier>(type_void_, 0, nullptr, empty_decl_);
  res->ref_->level_ = 1;
  res->ref_->type_ = type_void_;
  appendIns(block_top_, insCons(Type::INS_MALLOC, res, size, nullptr));
  return res;
}

bool Parser::canPass(std::shared_ptr<ReturnType> a,
                     std::shared_ptr<ReturnType> b) {
  if (canAssign(a, b)) {
    return true;
  }

  if ((a->ref_->array_ != nullptr) && (b->ref_->array_ != nullptr)) {
    std::shared_ptr<Array> aa = a->ref_->array_->nxt_;
    std::shared_ptr<Array> bb = b->ref_->array_->nxt_;
    while ((aa != nullptr) && (bb != nullptr)) {
      if (aa->num_ != bb->num_) {
        return false;
      }
      aa = aa->nxt_;
      bb = bb->nxt_;
    }
    return true;
  }

  return isOneDim(a) && isOneDim(b);
}

bool Parser::canAssign(std::shared_ptr<ReturnType> l,
                       std::shared_ptr<ReturnType> r) {
  if ((l->ref_->array_ != nullptr) || (r->ref_->array_ != nullptr)) {
    return false;
  }

  if ((l->ref_->type_ == r->ref_->type_) && (!l->ref_->level_) &&
      (!r->ref_->level_)) {
    return true;
  }

  if ((isInt(l)) && (isInt(r))) {
    return true;
  }

  if ((isInt(l) || l->ref_->level_) && (isInt(r) || r->ref_->level_)) {
    return true;
  }

  return false;
}

bool Parser::isOneDim(std::shared_ptr<ReturnType> th) {
  return ((th->ref_->level_) || ((th->ref_->array_ != nullptr) &&
                                 (th->ref_->array_->nxt_ == nullptr))) &&
         !((th->ref_->level_) && ((th->ref_->array_ != nullptr) &&
                                  (th->ref_->array_->nxt_ == nullptr)));
}

std::shared_ptr<ReturnType> makeFuncReturnType(std::shared_ptr<Function> func) {
  std::shared_ptr<ReturnType> res = std::shared_ptr<ReturnType>();
  res->func_ = func;
  std::shared_ptr<Identifier> type_id = std::make_shared<Identifier>(
      func->type_, 0, nullptr, std::make_shared<Declarator>());
  type_id->level_ = func->level_;
  type_id->type_ = func->type_;
  res->ref_ = type_id;
  return res;
}

std::shared_ptr<ReturnType> Parser::deltaMultipler(
    std::shared_ptr<ReturnType> th) {
  int width = 1;
  if (isInt(th)) {
    return const_one_;
  }
  if (th->ref_->array_ != nullptr) {
    width = th->ref_->array_->mul_;
    if (th->ref_->level_) {
      return makeConstReturnType(width * 4);
    } else {
      assert(th->ref_->type_->width_ != -1);
      return makeConstReturnType(width * th->ref_->type_->width_);
    }
  }
  if (th->ref_->level_ > 1) {
    return makeConstReturnType(4);
  } else {
    if (th->ref_->level_) {
      assert(th->ref_->type_->width_ != -1);
      width = th->ref_->type_->width_;
    } else {
      assert(false);
    }
  }

  return makeConstReturnType(width);
}

bool Parser::isPointer(std::shared_ptr<ReturnType> th) {
  return th->ref_->level_ || (th->ref_->array_ != nullptr);
}

bool Parser::isType() {
  if ((!tokens_->at(cur_).literal_.compare("int")) ||
      (!tokens_->at(cur_).literal_.compare("char")) ||
      (!tokens_->at(cur_).literal_.compare("void")) ||
      (!tokens_->at(cur_).literal_.compare("struct")) ||
      (!tokens_->at(cur_).literal_.compare("union"))) {
    return true;
  }

  if (tokens_->at(cur_).type_ != Type::IDENTIFIER) {
    return false;
  }

  std::shared_ptr<Environment> env = cur_env_;
  std::shared_ptr<Identifier> id = nullptr;
  while (env != nullptr) {
    id = findId(env->ids_, tokens_->at(cur_).literal_);
    if ((id != nullptr) && (!id->is_var_)) {
      return true;
      ;
    }
    env = env->pre_;
  }
  return false;
}

std::shared_ptr<ReturnType> Parser::loadAddress(
    std::shared_ptr<ReturnType> th) {
  std::shared_ptr<ReturnType> res = makeTmpReturnType();
  appendIns(block_top_, insCons(Type::INS_LD_ADDR, res, th, nullptr));
  res->ref_ = Identifier::cloneIdentifier(th->ref_);
  ++res->ref_->level_;
  return res;
}