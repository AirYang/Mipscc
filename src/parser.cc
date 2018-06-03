#include "parser.h"

Parser::Parser(std::shared_ptr<std::vector<Token>> tokens)
    : cur_(0), tokens_(tokens) {}

void Parser::parse() {
  while (cur_ < tokens_->size()) {
    auto look = tokens_->at(cur_);
    if (!look.literal_.compare("typedef")) {
    } else {
    }
  }
}