// mips cc include
#include "mips_cc.h"

// standard C
#include <cassert>
// standard C++
#include <algorithm>
#include <fstream>
// other include

// this project other include
#include "assembler.h"
#include "lexer.h"
#include "para_init.h"
#include "parser.h"

Mipscc::Mipscc(int argc, char** argv)
    : src_file_(),
      out_file_(),
      need_lexer_(false),
      need_parser_(false),
      need_assembler_(false),
      buffer_(std::make_shared<std::vector<char>>()) {
  // program parameter init
  ParaInit pinit(argc, argv);
  src_file_ = pinit.getSrcFile();
  out_file_ = pinit.getOutFile();
  need_lexer_ = pinit.needLexer();
  need_parser_ = pinit.needParser();
  need_assembler_ = pinit.needAssembler();

  // input only *.c file
  assert((src_file_.size() > 1) &&
         (src_file_.find_last_of(".c") == src_file_.size() - 1));

  // output only *.s file
  assert((out_file_.size() > 1) &&
         (out_file_.find_last_of(".s") == out_file_.size() - 1));

  // read buffer from src file
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
  auto parse_result = parser.parse();
  if (need_parser_) {
    parser.showIr();
  }

  // need out put file
  // global
  // func_header
  Assembler assembler(parse_result.first, parse_result.second);
  auto assemble_result = assembler.getMips();
  if (need_assembler_ && (assemble_result != nullptr)) {
    std::for_each(assemble_result->begin(), assemble_result->end(),
                  [](const char& c) { std::cout << c; });
  }
  writeOut(assemble_result);
}

void Mipscc::bufferInit() {
  std::ifstream ifst(src_file_);
  assert(ifst.is_open());
  for (char c; ifst.get(c); buffer_->push_back(c))
    ;
}

void Mipscc::writeOut(std::shared_ptr<std::string> out_buffer) {
  std::ofstream ofst(out_file_);
  assert(ofst.is_open());
  assert(out_buffer != nullptr);
  std::for_each(out_buffer->begin(), out_buffer->end(),
                [&ofst](const char& c) { ofst << c; });
  ofst.close();
}