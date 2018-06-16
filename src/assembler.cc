#include "assembler.h"

#include <string>
#include <vector>

#include "parser.h"

Assembler::Assembler(std::shared_ptr<Environment> global,
                     std::shared_ptr<Function> func_head)
    : oss_(),
      global_(global),
      func_head_(func_head),
      mipss_(std::make_shared<std::string>()) {}

std::shared_ptr<std::string> Assembler::getMips() {
  getMipsStatic();
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