// mips cc include
#include "mips_cc.h"

// standard C
#include <cassert>
// standard C++
#include <algorithm>
#include <fstream>
// other include

// this project other include
#include "lexer.h"
#include "para_init.h"
#include "parser.h"

Mipscc::Mipscc(int argc, char** argv)
    : file_(),
      need_lexer_(false),
      need_parser_(false),
      buffer_(std::make_shared<std::vector<char>>()) {
  // program parameter init
  ParaInit pinit(argc, argv);
  file_ = pinit.getFile();
  need_lexer_ = pinit.needLexer();
  need_parser_ = pinit.needParser();

  // input only *.c file
  assert((file_.size() > 1) && (file_.find_last_of(".c") == file_.size() - 1));

  // read buffer from file
  bufferInit();
}

void Mipscc::run() {
  Lexer lexer(buffer_);
  auto tokens = lexer.tokenize();
  if (need_lexer_) {
    std::for_each(tokens->begin(), tokens->end(), [](const Token& tk) {
      std::cout << tk.row_ << ", " << tk.col_ << ": " << tk.literal_
                << std::endl;
    });
  }

  Parser parser(tokens);
  parser.parse();
  if (need_parser_) {
    parser.showIr();
  }
}

void Mipscc::bufferInit() {
  std::ifstream ifst(file_);
  assert(ifst.is_open());
  for (char c; ifst.get(c); buffer_->push_back(c))
    ;
}