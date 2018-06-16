#include "assembler.h"

Assembler::Assembler(std::shared_ptr<Environment> global,
                     std::shared_ptr<Function> func_head)
    : global_(global),
      func_head_(func_head),
      mipss_(std::make_shared<std::vector<std::string>>()) {}

std::shared_ptr<std::vector<std::string>> Assembler::getMips() {
  return mipss_;
}
