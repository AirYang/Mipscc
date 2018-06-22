#include "assembler.h"

#include <cassert>

#include <string>
#include <vector>

#include "parser.h"

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::unary_op_literal_{{Type::INS_NEG, "-"}, {Type::INS_NOT, "~"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::unary_op_ins_{{Type::INS_NEG, "neg"}, {Type::INS_NOT, "not"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::binary_op_literal_{
        {Type::INS_ADD, "+"},  {Type::INS_SUB, "-"},   {Type::INS_MUL, "*"},
        {Type::INS_DIV, "/"},  {Type::INS_OR, "|"},    {Type::INS_XOR, "^"},
        {Type::INS_AND, "&"},  {Type::INS_SLLV, "<<"}, {Type::INS_SRLV, ">>"},
        {Type::INS_SNE, "!="}, {Type::INS_SEQ, "=="},  {Type::INS_SGT, ">"},
        {Type::INS_SLT, "<"},  {Type::INS_SGE, "<="},  {Type::INS_SLE, ">="},
        {Type::INS_REM, "mod"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::binary_op_ins_{
        {Type::INS_ADD, "add"},   {Type::INS_SUB, "sub"},
        {Type::INS_MUL, "mul"},   {Type::INS_DIV, "div"},
        {Type::INS_OR, "or"},     {Type::INS_XOR, "xor"},
        {Type::INS_AND, "and"},   {Type::INS_SLLV, "sllv"},
        {Type::INS_SRLV, "srlv"}, {Type::INS_SNE, "sne"},
        {Type::INS_SEQ, "seq"},   {Type::INS_SGT, "sgt"},
        {Type::INS_SLT, "slt"},   {Type::INS_SGE, "sge"},
        {Type::INS_SLE, "sle"},   {Type::INS_REM, "rem"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::btf_op_literal_{{Type::INS_BEQZ, "ifFalse"},
                               {Type::INS_BNEZ, "ifTrue"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::btf_op_ins_{{Type::INS_BEQZ, "beqz"}, {Type::INS_BNEZ, "bnez"}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::bb_op_literal_{{Type::INS_BLE, "<="}, {Type::INS_BGE, ">="},
                              {Type::INS_BLT, "<"},  {Type::INS_BGT, ">"},
                              {Type::INS_BNE, "!="}, {Type::INS_BEQ, "=="}};

const std::unordered_map<Type, std::string, EnumClassHash>
    Assembler::bb_op_ins_{{Type::INS_BLE, "ble"}, {Type::INS_BGE, "bge"},
                          {Type::INS_BLT, "blt"}, {Type::INS_BGT, "bgt"},
                          {Type::INS_BNE, "bne"}, {Type::INS_BEQ, "beq"}};

Assembler::Assembler(std::shared_ptr<Environment> global,
                     std::shared_ptr<Function> func_head)
    : oss_(),
      global_(global),
      func_head_(func_head),
      mipss_(std::make_shared<std::string>()),
      regs_(),
      para_offset_(0),
      bfs_cnt_(0) {
  // reg init
  for (size_t i = 0; i < 10; ++i) {
    Register reg;
    reg.literal_ = "$t" + std::to_string(i);
    reg.ref_ = nullptr;
    regs_.push_back(reg);
  }

  for (size_t i = 10; i < 18; ++i) {
    Register reg;
    reg.literal_ = "$s" + std::to_string(i - 10);
    reg.ref_ = nullptr;
    regs_.push_back(reg);
  }

  for (size_t i = 18; i < 21; ++i) {
    Register reg;
    reg.literal_ = "$a" + std::to_string(i - 17);
    reg.ref_ = nullptr;
    regs_.push_back(reg);
  }
}

std::shared_ptr<std::string> Assembler::getMips() {
  // data段
  getMipsStatic();
  oss_ << ".text\n";

  ++bfs_cnt_;
  std::shared_ptr<Function> func = func_head_;
  std::vector<int> blck_hsh(Block::blck_cnt_ + 1, 0);

  while (func != nullptr) {
    if (func->block_ != nullptr) {
      int stack_size = 4;
      int ra = 0;
      for (std::shared_ptr<ReturnType> i = func->regs_; i != nullptr;
           i = i->nxt_) {
        if (i->reg_num_ == -1) {
          if (i->ref_->env_belong_ == global_) {
          } else {
            if (i->ref_->id_.size() > 0) {
              if (i->ref_->arg_num_ != -1) {
                stack_size += paraWidth(i);
              } else {
                stack_size += Parser::varWidth(i->ref_);
              }
            }
          }
        } else {
          stack_size += 4;
        }
      }
      ra = stack_size;
      for (std::shared_ptr<ReturnType> i = func->regs_; i != nullptr;
           i = i->nxt_) {
        if (i->ref_->arg_num_ != -1) {
          std::shared_ptr<ReturnType> j = func->regs_;
          while (j != nullptr) {
            if ((j->ref_->arg_num_ != -1) &&
                (j->ref_->arg_num_ <= i->ref_->arg_num_)) {
              i->sp_offset_ -= paraWidth(j);
            }
            j = j->nxt_;
          }
          i->sp_offset_ += stack_size;
          if (i->sp_offset_ < ra) {
            ra = i->sp_offset_;
          }
        }
      }
      ra -= 4;
      int loc = ra;
      for (std::shared_ptr<ReturnType> i = func->regs_; i != nullptr;
           i = i->nxt_) {
        if ((i->ref_->arg_num_ == -1) && (i->ref_->env_belong_ != global_)) {
          if (i->reg_num_ == -1) {
            loc -= Parser::varWidth(i->ref_);
            i->sp_offset_ = loc;
          } else if (i->reg_num_ != -1) {
            loc -= 4;
            i->sp_offset_ = loc;
          }
        }
      }
      for (std::shared_ptr<ReturnType> i = func->regs_; i != nullptr;
           i = i->nxt_) {
        if (i->reg_num_ == -1) {
          if (i->ref_->env_belong_ == global_) {
            oss_ << "\t# global variable " << i->ref_->id_ << "\n";
          } else {
            if (i->ref_->arg_num_ != -1) {
              oss_ << "\t# argument " << i->ref_->id_ << " "
                   << i->ref_->arg_num_ << " offset: " << i->sp_offset_ << "\n";
            } else {
              oss_ << "\t# normal variable " << i->ref_->id_
                   << " offset: " << i->sp_offset_ << "\n";
            }
          }
        } else {
          oss_ << "\t# tmp variable $" << i->reg_num_ << " offset "
               << i->sp_offset_ << "\n";
        }
      }

      if (!func->id_.compare("main")) {
        oss_ << "main:\n";
      } else {
        oss_ << "__func" << func->id_ << ":\n";
      }

      oss_ << "\t# $ra @ " << ra << "($sp)\n";
      oss_ << "\t# stack size " << stack_size << "\n";
      oss_ << "\tadd $sp, -" << stack_size << "\n";
      oss_ << "\tusw $ra, " << ra << "($sp)\n";

      std::vector<std::shared_ptr<Block>> opt_que;
      opt_que.push_back(func->block_);
      blck_hsh[func->block_->id_] = bfs_cnt_;
      for (size_t f = 0; f < opt_que.size(); ++f) {
        std::shared_ptr<Block> a = opt_que[f]->condi_;
        std::shared_ptr<Block> b = opt_que[f]->non_condi_;

        if ((a == nullptr) && (b == nullptr) && (opt_que[f] != func->end_)) {
          opt_que[f]->non_condi_ = func->end_;
          b = func->end_;
        }

        if (a != nullptr) {
          if (blck_hsh[a->id_] != bfs_cnt_) {
            opt_que.push_back(a);
            blck_hsh[a->id_] = bfs_cnt_;
          }
        }

        if (b != nullptr) {
          if (blck_hsh[b->id_] != bfs_cnt_) {
            opt_que.push_back(b);
            blck_hsh[b->id_] = bfs_cnt_;
          }
        }

        getMipsBlock(opt_que[f]);

        if (opt_que[f] == func->end_) {
          oss_ << "\t # end of function real return\n";
          oss_ << "\tulw $ra, " << ra << "($sp)\n";
          oss_ << "\tadd $sp, " << stack_size << "\n";
          if (!func->id_.compare("main")) {
            oss_ << "\t # end of the program\n";
            oss_ << "\tli $v0, 17\n";
            oss_ << "\tli $a0, 0\n";
            oss_ << "\tsyscall\n";
          } else {
            oss_ << "\tjr $ra\n";
          }
        }
      }
      oss_ << "\n";
    }
    func = func->nxt_;
  }

  *mipss_ = oss_.str();
  return mipss_;
}

void Assembler::getMipsStatic() {
  oss_ << ".data\n";
  for (std::shared_ptr<Identifier> iter = global_->ids_; iter != nullptr;
       iter = iter->nxt_) {
    if (iter->is_var_) {
      if ((iter->id_.size() > 0) && (iter->id_[0] == '$')) {
        oss_ << "\t" << iter->id_ << ":\t";
      } else {
        oss_ << "\t__" << iter->id_ << ":\t";
      }

      if (iter->init_type_ == INIT_STR) {
        oss_ << ".asciiz\t" << iter->init_str_ << "\n";
      } else if (iter->init_type_ == INIT_NONE) {
        oss_ << ".space\t" << Parser::varWidth(iter) << "\n";
      } else {
        int length = 0;
        if (iter->array_ != nullptr) {
          length = iter->array_->mul_ * iter->array_->num_;
        } else {
          length = 1;
        }

        std::vector<std::shared_ptr<InitPair>> initlist;
        for (std::shared_ptr<InitPair> initpair = iter->init_list_;
             initpair != nullptr; initpair = initpair->nxt_) {
          initlist.push_back(initpair);
        }

        if ((iter->type_ != nullptr) &&
            (!iter->type_->literal_.compare("char")) && (iter->level_ == 0)) {
          oss_ << ".byte\t";
        } else {
          oss_ << ".word\t";
        }

        for (int i = 0; i < length; ++i) {
          if (initlist[i] != nullptr) {
            if (initlist[i]->label_.size() != 0) {
              oss_ << realGlobal(initlist[i]->label_);
            } else {
              oss_ << initlist[i]->num_;
            }
          } else {
            oss_ << "0";
          }
          if (i + 1 < length) {
            oss_ << ",";
          } else {
            oss_ << "\n";
          }
        }
      }
    }
  }
  oss_ << ".globl main\n";
}

std::string Assembler::realGlobal(const std::string& str) {
  if ((str.size() > 0) && (str[0] == '$')) {
    return str;
  } else {
    return "__" + str;
  }
}

int Assembler::paraWidth(std::shared_ptr<ReturnType> th) {
  if ((th->ref_->array_ != nullptr) || (th->ref_->level_)) {
    return 4;
  } else if (!th->ref_->type_->literal_.compare("int")) {
    return 4;
  } else {
    return th->ref_->type_->width_;
  }
}

void Assembler::getMipsBlock(std::shared_ptr<Block> blck) {
  oss_ << "__label" << blck->id_ << ":\n";
  for (size_t i = 0; i < regs_.size(); ++i) {
    regs_[i].ref_ = nullptr;
  }
  for (size_t i = 0; i < blck->ins_.size(); ++i) {
    std::shared_ptr<ReturnType> des = blck->ins_[i].des_;
    std::shared_ptr<ReturnType> a = blck->ins_[i].a_;
    std::shared_ptr<ReturnType> b = blck->ins_[i].b_;

    if (des != nullptr) {
      des->reg_ = nullptr;
    }

    if (a != nullptr) {
      a->reg_ = nullptr;
    }

    if (b != nullptr) {
      b->reg_ = nullptr;
    }
  }

  for (size_t i = 0; i < blck->ins_.size(); ++i) {
    std::shared_ptr<ReturnType> des = blck->ins_[i].des_;
    std::shared_ptr<ReturnType> a = blck->ins_[i].a_;
    std::shared_ptr<ReturnType> b = blck->ins_[i].b_;
    if ((des != nullptr) && (des->reg_ == nullptr)) {
    }

    if (a != nullptr) {
    }

    if (b != nullptr) {
    }
  }

  for (size_t i = 0; i < blck->ins_.size(); ++i) {
    oss_ << "\t";
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>();
    *ins = blck->ins_[i];
    getMipsInstruction(ins);
    if (IsBranch(blck->ins_[i].ins_)) {
      oss_ << "__label" << blck->condi_->id_ << "\n";
      if (i + 1 != blck->ins_.size()) {
        std::shared_ptr<Instruction> ins_next = std::make_shared<Instruction>();
        *ins_next = blck->ins_[i + 1];
        getMipsInstruction(ins_next);
        assert(false);
      }
    }
  }
  if (blck->non_condi_ != nullptr) {
    oss_ << "\tj __label" << blck->non_condi_->id_ << "\n";
  } else {
    std::shared_ptr<Function> iter = func_head_;
    while (iter != nullptr) {
      if (blck == iter->end_) {
        return;
      }
      iter = iter->nxt_;
    }
    assert(false);
  }
}

void Assembler::getMipsInstruction(std::shared_ptr<Instruction> ins) {
  if (IsUnary(ins->ins_)) {
    oss_ << "# " << ReturnType::toString(ins->des_) << " = "
         << unary_op_literal_.at(ins->ins_) << ReturnType::toString(ins->a_)
         << "\n";
    insLoadValue("$t1", ins->a_);
    oss_ << "\t " << unary_op_ins_.at(ins->ins_) << " $t0, $t1\n";
    insStoreBack(ins->des_, "$t0");
  } else if (Type::INS_LD_ADDR == ins->ins_) {
    oss_ << "# " << ReturnType::toString(ins->des_) << " = &"
         << ReturnType::toString(ins->a_) << "\n";
    insLoadAddr("$t1", ins->a_);
    insStoreBack(ins->des_, "$t1");
  } else if (IsBinary(ins->ins_)) {
    oss_ << "# " << ReturnType::toString(ins->des_) << " = "
         << ReturnType::toString(ins->a_) << " "
         << binary_op_literal_.at(ins->ins_) << " "
         << ReturnType::toString(ins->b_) << "\n";
    insLoadValue("$t1", ins->a_);
    insLoadValue("$t2", ins->b_);
    oss_ << "\t" << binary_op_ins_.at(ins->ins_) << " $t0, $t1, $t2\n";
    insStoreBack(ins->des_, "$t0");
  } else if (IsBtf(ins->ins_)) {
    oss_ << "# " << btf_op_literal_.at(ins->ins_) << " "
         << ReturnType::toString(ins->a_) << " goto \n";
    insLoadValue("$t0", ins->a_);
    oss_ << "\t" << btf_op_ins_.at(ins->ins_) << " $t0, ";
  } else if (Type::INS_PARA == ins->ins_) {
    oss_ << "# para " << ReturnType::toString(ins->a_) << "\n";
    para_offset_ += paraWidth(ins->b_);
    oss_ << "\t# " << para_offset_ << "($sp)\n";
    insLoadValue("$t0", ins->a_);
    if (Parser::isPointer(ins->b_) || Parser::isIntStatic(ins->b_)) {
      if ((ins->b_->ref_->level_ == 0) && (ins->b_->ref_->type_ != nullptr) &&
          (!ins->b_->ref_->type_->literal_.compare("char"))) {
        oss_ << "\tsb $t0, -" << para_offset_ << "($sp)\n";
      } else {
        oss_ << "\tusw $t0, -" << para_offset_ << "($sp)\n";
      }
    } else {
      for (int i = 0; i < ins->b_->ref_->type_->width_; i += 4) {
        oss_ << "\tulw $v1, " << i << "($t0)\n";
        oss_ << "\tusw $v1, " << i - para_offset_ << "($sp)\n";
      }
    }
  } else if (Type::INS_CALL == ins->ins_) {
    para_offset_ = 0;
    if (ins->des_ != nullptr) {
      oss_ << "# call " << ReturnType::toString(ins->des_) << " "
           << ReturnType::toString(ins->a_) << "\n";
      oss_ << "\tjal __func" << ins->a_->func_->id_ << "\n";
      if (fitInWord(ins->des_)) {
        insStoreBack(ins->des_, "$v0");
      } else {
        insLoadValue("$t1", ins->des_);
        for (int i = 0; i < ins->des_->ref_->type_->width_; i += 4) {
          oss_ << "\tulw $v1, " << i << "($v0)\n";
          oss_ << "\tusw $v1, " << i << "($t1)\n";
        }
      }
    } else {
      oss_ << "# call " << ReturnType::toString(ins->a_) << "\n";
      oss_ << "\tjal __func" << ins->a_->func_->id_ << "\n";
    }
  } else if (Type::INS_RET == ins->ins_) {
    if (ins->a_ != nullptr) {
      oss_ << "# ret " << ReturnType::toString(ins->a_) << "\n";
      insLoadValue("$v0", ins->a_);
    } else {
      oss_ << "# ret \n";
    }
  } else if (Type::INS_HALT == ins->ins_) {
    oss_ << "# exit \n";
    insLoadValue("$a0", ins->a_);
    oss_ << "\tli $v0, 17\n";
    oss_ << "\tsyscall\n";
  } else if (Type::INS_MOVE == ins->ins_) {
    oss_ << "# " << ReturnType::toString(ins->des_) << " <- "
         << ReturnType::toString(ins->a_)
         << ", width: " << ins->des_->ref_->type_->width_ << " move\n";
    insLoadValue("$t1", ins->a_);
    if (Parser::isIntStatic(ins->des_) || ins->des_->ref_->level_) {
      insStoreBack(ins->des_, "$t1");
    } else {
      insLoadValue("$t2", ins->des_);
      for (int i = 0; i < ins->des_->ref_->type_->width_; i += 4) {
        oss_ << "\tulw $v1, " << i << "($t1)\n";
        oss_ << "\tusw $v1, " << i << "($t2)\n";
      }
    }
  } else if (Type::INS_MALLOC == ins->ins_) {
    oss_ << "# malloc " << ReturnType::toString(ins->des_) << ", "
         << ReturnType::toString(ins->a_) << "\n";
    insLoadValue("$a0", ins->a_);
    oss_ << "\tli $v0, 9\n";
    oss_ << "\tsyscall\n";
    insStoreBack(ins->des_, "$v0");
  } else if (Type::INS_PRINT_INT == ins->ins_) {
    oss_ << "# printint " << ReturnType::toString(ins->a_) << "\n";
    insLoadValue("$a0", ins->a_);
    oss_ << "\tli $v0, 1\n";
    oss_ << "\tsyscall\n";
  } else if (Type::INS_PRINT_STRING == ins->ins_) {
    oss_ << "# printstring " << ReturnType::toString(ins->a_) << "\n";
    insLoadValue("$a0", ins->a_);
    oss_ << "\tli $v0, 4\n";
    oss_ << "\tsyscall\n";
  } else if (Type::INS_PUTCHAR == ins->ins_) {
    oss_ << "# putchar " << ReturnType::toString(ins->a_) << "\n";
    insLoadValue("$a0", ins->a_);
    oss_ << "\tli $v0, 11\n";
    oss_ << "\tsyscall\n";
  } else if (Type::INS_GETCHAR == ins->ins_) {
    oss_ << "# getchar " << ReturnType::toString(ins->des_) << "\n";
    oss_ << "\tli $v0, 12\n";
    oss_ << "\tsyscall\n";
    insStoreBack(ins->des_, "$v0");
  } else if (Type::INS_ARRAY_READ == ins->ins_) {
    oss_ << "# " << ReturnType::toString(ins->des_) << " = *"
         << ReturnType::toString(ins->a_) << "\n";
    insLoadValue("$t1", ins->a_);
    if ((ins->a_->ref_->type_ != nullptr) &&
        (!ins->a_->ref_->type_->literal_.compare("char")) &&
        (!ins->a_->ref_->level_)) {
      oss_ << "\tlb $t0, 0($t1)\n";
    } else {
      oss_ << "\tulw $t0, 0($t1)\n";
    }
    insStoreBack(ins->des_, "$t0");
  } else if (Type::INS_ARRAY_WRITE == ins->ins_) {
    oss_ << "# *" << ReturnType::toString(ins->a_) << " = "
         << ReturnType::toString(ins->b_) << "\n";
    insLoadValue("$t1", ins->b_);
    insLoadValue("$t0", ins->a_);
    if ((ins->a_->ref_->type_ != nullptr) &&
        (!ins->a_->ref_->type_->literal_.compare("char")) &&
        (!ins->a_->ref_->level_)) {
      oss_ << "\tsb $t1, 0($t0)\n";
    } else {
      oss_ << "\tusw $t1, 0($t0)\n";
    }
  } else if (IsBb(ins->ins_)) {
    oss_ << "# if " << ReturnType::toString(ins->a_) << " "
         << bb_op_literal_.at(ins->ins_) << " " << ReturnType::toString(ins->b_)
         << " goto\n";
    insLoadValue("$t1", ins->a_);
    insLoadValue("$t2", ins->b_);
    oss_ << "\t" << bb_op_ins_.at(ins->ins_) << " $t1, $t2, ";
  } else {
    oss_ << TypeToStr(ins->ins_) << "\n";
    assert(false);
  }
}

int Assembler::fitInWord(std::shared_ptr<ReturnType> th) {
  if (th->reg_num_ != -1) {
    return 4;
  }

  if (th->ref_->array_ != nullptr) {
    if (th->ref_->arg_num_ != -1) {
      return 4;
    }
    return 0;
  }

  if (th->ref_->level_) {
    return 4;
  }

  if ((th->ref_->type_ != nullptr) &&
      (!th->ref_->type_->literal_.compare("int"))) {
    return 4;
  }

  if ((th->ref_->type_ != nullptr) &&
      (!th->ref_->type_->literal_.compare("char"))) {
    return 1;
  }

  return 0;
}

void Assembler::insLoadValue(const std::string& reg,
                             std::shared_ptr<ReturnType> th) {
  if (th->ret_type_ == CONST_VAL) {
    oss_ << "\tli " << reg << ", " << th->const_val_ << "\n";
    return;
  }

  int fitSize = fitInWord(th);
  if (th->reg_num_ == -1) {
    if (th->ref_->env_belong_ == global_) {
      if (fitSize) {
        if (fitSize == 1) {
          oss_ << "\tlb " << reg << ", " << realGlobal(th->ref_->id_) << "\n";
        } else {
          oss_ << "\tulw " << reg << ", " << realGlobal(th->ref_->id_) << "\n";
        }
      } else {
        oss_ << "\tla " << reg << ", " << realGlobal(th->ref_->id_) << "\n";
      }
    } else {
      if (fitSize) {
        if (fitSize == 1) {
          oss_ << "\tlb " << reg << ", " << th->sp_offset_ << "($sp)\n";
        } else {
          oss_ << "\tulw " << reg << ", " << th->sp_offset_ << "($sp)\n";
        }
      } else {
        if ((th->ref_->arg_num_ != -1) && (th->ref_->array_ != nullptr)) {
          oss_ << "\tulw " << reg << ", " << th->sp_offset_ << "($sp)\n";
        } else {
          oss_ << "\tla " << reg << ", " << th->sp_offset_ << "($sp)\n";
        }
      }
    }
  } else {
    oss_ << "\tulw " << reg << ", " << th->sp_offset_ << "($sp)\n";
  }
}

void Assembler::insStoreBack(std::shared_ptr<ReturnType> th,
                             const std::string& reg) {
  if (th->reg_num_ == -1) {
    if (th->ref_->env_belong_ == global_) {
      if (fitInWord(th) == 1) {
        oss_ << "\tsb " << reg << ", " << th->ref_->id_ << "\n";
      } else {
        oss_ << "\tusw " << reg << ", " << realGlobal(th->ref_->id_) << "\n";
      }
    } else {
      if (fitInWord(th) == 1) {
        oss_ << "\tsb " << reg << ", " << th->sp_offset_ << "($sp)\n";
      } else {
        oss_ << "\tusw " << reg << ", " << th->sp_offset_ << "($sp)\n";
      }
    }
  } else {
    oss_ << "\tusw " << reg << ", " << th->sp_offset_ << "($sp)\n";
  }
}

void Assembler::insLoadAddr(const std::string& reg,
                            std::shared_ptr<ReturnType> th) {
  if (th->reg_num_ == -1) {
    if (th->ref_->env_belong_ == global_) {
      oss_ << "\tla " << reg << ", " << realGlobal(th->ref_->id_) << "\n";
    } else {
      oss_ << "\tla " << reg << ", " << th->sp_offset_ << "($sp)\n";
    }
  } else {
    oss_ << "\tulw " << reg << ", " << th->sp_offset_ << "($sp)\n";
  }
}