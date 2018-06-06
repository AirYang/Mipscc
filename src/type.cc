#include "type.h"

const char* TypeToStr(Type tp) {
#define TYPE_TO_STR(x) \
  case x:              \
    return (#x);

  switch (tp) {
    // FIVE KIND
    TYPE_TO_STR(Type::KEY)
    TYPE_TO_STR(Type::IDENTIFIER)
    TYPE_TO_STR(Type::INT_CONST)
    TYPE_TO_STR(Type::CHAR_CONST)
    TYPE_TO_STR(Type::STRING_CONST)
    TYPE_TO_STR(Type::OPERATION)
    // OPERATION
    TYPE_TO_STR(Type::INS_ADD)
    TYPE_TO_STR(Type::INS_SUB)
    TYPE_TO_STR(Type::INS_MUL)
    TYPE_TO_STR(Type::INS_DIV)
    TYPE_TO_STR(Type::INS_OR)
    TYPE_TO_STR(Type::INS_XOR)
    TYPE_TO_STR(Type::INS_AND)
    TYPE_TO_STR(Type::INS_SLLV)
    TYPE_TO_STR(Type::INS_SRLV)
    TYPE_TO_STR(Type::INS_SNE)
    TYPE_TO_STR(Type::INS_SEQ)
    TYPE_TO_STR(Type::INS_SGT)
    TYPE_TO_STR(Type::INS_SLT)
    TYPE_TO_STR(Type::INS_SGE)
    TYPE_TO_STR(Type::INS_SLE)
    TYPE_TO_STR(Type::INS_REM)
    TYPE_TO_STR(Type::INS_NEG)
    TYPE_TO_STR(Type::INS_BNEZ)
    TYPE_TO_STR(Type::INS_BEQZ)
    TYPE_TO_STR(Type::INS_PARA)
    TYPE_TO_STR(Type::INS_CALL)
    TYPE_TO_STR(Type::INS_PRINT_INT)
    TYPE_TO_STR(Type::INS_PRINT_STRING)
    TYPE_TO_STR(Type::INS_MALLOC)
    TYPE_TO_STR(Type::INS_GETCHAR)
    TYPE_TO_STR(Type::INS_PUTCHAR)
    TYPE_TO_STR(Type::INS_MOVE)
    TYPE_TO_STR(Type::INS_NOT)
    TYPE_TO_STR(Type::INS_LD_ADDR)
    TYPE_TO_STR(Type::INS_RET)
    TYPE_TO_STR(Type::INS_HALT)
    TYPE_TO_STR(Type::INS_ARRAY_WRITE)
    TYPE_TO_STR(Type::INS_ARRAY_READ)
    TYPE_TO_STR(Type::INS_BLE)
    TYPE_TO_STR(Type::INS_BGE)
    TYPE_TO_STR(Type::INS_BLT)
    TYPE_TO_STR(Type::INS_BGT)
    TYPE_TO_STR(Type::INS_BNE)
    TYPE_TO_STR(Type::INS_BEQ)
    // not a kind
    TYPE_TO_STR(Type::NOT_A_TYPE)
  }

#undef TYPE_TO_STR
  return "Unsupported Type";
}

Environment::Environment(std::shared_ptr<Environment> pre)
    : pre_(pre), types_(nullptr), ids_(nullptr) {}

PType::PType()
    : is_struct_(0), width_(-1), literal_(""), nxt_(nullptr), mem_(nullptr) {}

Declarator::Declarator()
    : literal_(""),
      level_(0),
      nxt_(nullptr),
      is_func_(0),
      args_(nullptr),
      dim_(nullptr) {}

std::shared_ptr<Declarator> Declarator::mergeDecl(
    std::shared_ptr<Declarator> a, std::shared_ptr<Declarator> b) {
  if (a == nullptr) {
    return b;
  }

  if (b == nullptr) {
    return a;
  }

  std::shared_ptr<Declarator> res = std::make_shared<Declarator>();
  res->literal_ = b->literal_;
  res->level_ = a->level_ + b->level_;
  res->nxt_ = nullptr;
  res->is_func_ = b->is_func_;
  res->args_ = b->args_;

  std::shared_ptr<Array> iter = a->dim_;
  std::shared_ptr<Array> tmp = nullptr;
  std::shared_ptr<Array> tail = nullptr;
  if (a->dim_ != nullptr) {
    tail = res->dim_ = std::make_shared<Array>();
    res->dim_->num_ = a->dim_->num_;
    iter = iter->nxt_;
    while (iter != nullptr) {
      tmp = std::make_shared<Array>();
      tmp->num_ = iter->num_;
      tail->nxt_ = tmp;
      tmp->pre_ = tail;
      tail = tmp;
      iter = iter->nxt_;
    }

    if (b->dim_ != nullptr) {
      b->dim_->pre_ = tail;
      tail->nxt_ = b->dim_;
    }

    tail = tail->pre_;

    while (tail != nullptr) {
      tail->mul_ = tail->nxt_->mul_ * tail->num_;
      tail = tail->pre_;
    }

  } else {
    res->dim_ = b->dim_;
  }
  return res;
}

ReturnType::ReturnType()
    : ret_type_(0),
      is_left_(0),
      const_val_(0),
      func_(nullptr),
      nxt_(nullptr),
      ref_(nullptr),
      reg_num_(-1),
      sp_offset_(0),
      belong_(nullptr),
      reg_(nullptr) {}

Instruction::Instruction()
    : ins_(Type::NOT_A_TYPE),
      ord_(0),
      des_(nullptr),
      a_(nullptr),
      b_(nullptr) {}

Function::Function(std::shared_ptr<PType> type, std::shared_ptr<Declarator> a,
                   std::shared_ptr<Declarator> b)
    : type_(type) {
  std::shared_ptr<Declarator> c = Declarator::mergeDecl(a, b);
  level_ = c->level_;
  id_ = c->literal_;
  args_ = c->args_;
  nxt_ = nullptr;
  block_ = nullptr;
  end_ = std::make_shared<Block>();
  regs_ = nullptr;
}

Array::Array() : num_(0), mul_(1), nxt_(nullptr), pre_(nullptr) {}

Identifier::Identifier(std::shared_ptr<PType> type, int is_var,
                       std::shared_ptr<Declarator> a,
                       std::shared_ptr<Declarator> b) {
  std::shared_ptr<Declarator> c = Declarator::mergeDecl(a, b);
  arg_num_ = -1;
  id_ = c->literal_;
  level_ = c->level_;
  type_ = type;
  from_ = 0;
  is_var_ = is_var;
  nxt_ = nullptr;
  array_ = c->dim_;
  init_type_ = INIT_NONE;
  init_str_ = str_val_ = "";
  init_list_ = nullptr;
}

Identifier::Identifier() {}

std::shared_ptr<Identifier> Identifier::cloneIdentifier(
    std::shared_ptr<Identifier> id) {
  std::shared_ptr<Identifier> res = std::make_shared<Identifier>();
  res->type_ = id->type_;
  res->level_ = id->level_;
  res->array_ = id->array_;
  res->env_belong_ = nullptr;
  res->nxt_ = nullptr;
  res->arg_num_ = -1;
  res->from_ = 0;
  res->id_ = "";
  res->init_list_ = nullptr;
  res->init_str_ = "";
  res->init_type_ = INIT_NONE;
  res->type_belong_ = nullptr;
  return res;
}

int Block::blck_cnt_ = 0;

Block::Block()
    : id_(++blck_cnt_),
      in_deg_(0),
      // ins_size_(0),
      // buffer_size_(0),
      ins_(),
      non_condi_(nullptr),
      condi_(nullptr) {}

InitPair::InitPair(int idx, int num, const std::string& label)
    : pos_(idx), num_(num), label_(label), nxt_(nullptr) {}