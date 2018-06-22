#include "assembler.h"

#include <string>
#include <vector>

#include "parser.h"

Assembler::Assembler(std::shared_ptr<Environment> global,
                     std::shared_ptr<Function> func_head)
    : oss_(),
      global_(global),
      func_head_(func_head),
      mipss_(std::make_shared<std::string>()),
      regs_() {
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

void Assembler::getMipsBlock(std::shared_ptr<Block> blck) {}

void Assembler::getMipsInstruction(std::shared_ptr<Instruction> ins) {}