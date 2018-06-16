#include "para_init.h"

ParaInit::ParaInit(int argc, char** argv) : parser_(argc, argv) {
  parserInit();
  parser_.run_and_exit_if_error();
}

void ParaInit::parserInit() {
  parser_.set_optional<bool>("l", "lexer", false,
                             "Need print lexer result (tokens)");
  parser_.set_optional<bool>("p", "parser", false,
                             "Nedd print parser result (irs)");
  parser_.set_optional<bool>("a", "assembler", false,
                             "Nedd print assembler result (mips)");
  parser_.set_required<std::string>("f", "files", "Input file [.c]");
  parser_.set_required<std::string>("o", "output", "Output file [.s]");
}

std::string ParaInit::getSrcFile() { return parser_.get<std::string>("f"); }

std::string ParaInit::getOutFile() { return parser_.get<std::string>("o"); }

bool ParaInit::needLexer() { return parser_.get<bool>("l"); }

bool ParaInit::needParser() { return parser_.get<bool>("p"); }

bool ParaInit::needAssembler() { return parser_.get<bool>("a"); }