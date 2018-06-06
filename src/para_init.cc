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
  parser_.set_required<std::string>("f", "files", "Input files [.c]");
}

std::string ParaInit::getFile() { return parser_.get<std::string>("f"); }

bool ParaInit::needLexer() { return parser_.get<bool>("l"); }

bool ParaInit::needParser() { return parser_.get<bool>("p"); }