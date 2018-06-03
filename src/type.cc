#include "type.h"

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
    : ins_(0), ord_(0), des_(nullptr), a_(nullptr), b_(nullptr) {}

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

int Block::blck_cnt_ = 0;

Block::Block()
    : id_(++blck_cnt_),
      in_deg_(0),
      ins_size_(0),
      buffer_size_(0),
      ins_(nullptr),
      non_condi_(nullptr),
      condi_(nullptr) {}