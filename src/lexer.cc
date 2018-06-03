#include "lexer.h"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <sstream>

Lexer::Lexer(std::shared_ptr<std::vector<char>> buffer)
    : cur_(0),
      col_(1),
      row_(1),
      buffer_(std::make_shared<std::vector<char>>()) {
  bufferInit(buffer);
}

std::shared_ptr<std::vector<Token>> Lexer::tokenize() {
  std::shared_ptr<std::vector<Token>> res =
      std::make_shared<std::vector<Token>>();

  while (cur_ < buffer_->size()) {
    res->push_back(nextToken());
  }

  return res;
}

void Lexer::bufferInit(std::shared_ptr<std::vector<char>> buffer) {
  std::shared_ptr<std::vector<char>> p = buffer_;
  std::string initcode =
      "void __my_fake_printf__(char *format, int *args) {\
	int idx = 0;\
	while (*format) {\
		if (*format == '%') {\
			if (format[1] == 'd') {\
				__print_int__(args[idx++]);\
			} else if (format[1] == 'c') {\
				putchar(args[idx++]);\
			} else if (format[1] == 's') {\
				__print_string__(args[idx++]);\
			} else if (format[1] == '%') {\
				putchar('%');\
			} else if (format[1] == '0') {\
				int width = format[2] - '0', x = args[idx++], len = 0, y;\
				y = x;\
				if (x < 0) {\
					x = -x;\
					y = -y;\
					putchar('-');\
					--width;\
				}\
				while (x) {\
					x /= 10;\
					--width;\
				}\
				while (width > 0) {\
					putchar('0');\
					--width;\
				}\
				__print_int__(y);\
				format += 2;\
			} else {\
				int width = format[2] - '0', x = args[idx++], len = 0, y;\
				y = x;\
				if (x < 0) {\
					x = -x;\
					y = -y;\
					putchar('-');\
				}\
				while (x) {\
					x /= 10;\
					--width;\
				}\
				while (width > 0) {\
					putchar('0');\
					--width;\
				}\
				__print_int__(y);\
				format += 2;\
			}\
			++format;\
		} else {\
			putchar(*format);\
		}\
		++format;\
	}\
}\n";

  std::for_each(initcode.begin(), initcode.end(),
                [&p](char c) { p->push_back(c); });
  std::for_each(buffer->begin(), buffer->end(),
                [&p](char c) { p->push_back(c); });
}

void Lexer::jumpChars(size_t num) {
  for (; num--;) {
    if (buffer_->at(cur_) == '\n') {
      ++row_;
      col_ = 1;
    } else {
      ++col_;
    }
    ++cur_;
  }
}

void Lexer::jumpUnuseChars() {
  for (size_t tmp = buffer_->size(); tmp != cur_;) {
    tmp = cur_;
    while ((cur_ < buffer_->size()) &&
           ((buffer_->at(cur_) == '\n') || (buffer_->at(cur_) == '\t') ||
            (buffer_->at(cur_) == ' ') || (buffer_->at(cur_) == '\r'))) {
      jumpChars(1);
    }

    if ((cur_ < buffer_->size()) && (buffer_->at(cur_) == '#')) {
      while ((cur_ < buffer_->size()) && (buffer_->at(cur_) != '\n')) {
        jumpChars(1);
      }
      jumpChars(1);
    }

    if ((cur_ < buffer_->size()) && (buffer_->at(cur_) == '/') &&
        (buffer_->at(cur_ + 1) == '/')) {
      while ((cur_ < buffer_->size()) && (buffer_->at(cur_) != '\n')) {
        jumpChars(1);
      }
      jumpChars(1);
    }

    if ((cur_ < buffer_->size()) && (buffer_->at(cur_) == '/') &&
        (buffer_->at(cur_ + 1) == '*')) {
      jumpChars(2);
      while ((buffer_->at(cur_) != '*') || (buffer_->at(cur_ + 1) != '/')) {
        jumpChars(1);
      }
      jumpChars(2);
    }
  }
}

Token Lexer::nextToken() {
  jumpUnuseChars();
  size_t ans = 0;
  Token res;
  res.row_ = row_;
  res.col_ = col_;

  if (strStartWith("void") && (4 > ans)) {
    ans = 4;
    res.type_ = Type::KEY;
  }

  if (strStartWith("char") && (4 > ans)) {
    ans = 4;
    res.type_ = Type::KEY;
  }

  if (strStartWith("int") && (3 > ans)) {
    ans = 3;
    res.type_ = Type::KEY;
  }

  if (strStartWith("typedef") && (7 > ans)) {
    ans = 7;
    res.type_ = Type::KEY;
  }

  if (strStartWith("for") && (3 > ans)) {
    ans = 3;
    res.type_ = Type::KEY;
  }

  if (strStartWith("continue") && (8 > ans)) {
    ans = 8;
    res.type_ = Type::KEY;
  }

  if (strStartWith("struct") && (6 > ans)) {
    ans = 6;
    res.type_ = Type::KEY;
  }

  if (strStartWith("return") && (6 > ans)) {
    ans = 6;
    res.type_ = Type::KEY;
  }

  if (strStartWith("sizeof") && (6 > ans)) {
    ans = 6;
    res.type_ = Type::KEY;
  }

  if (strStartWith("union") && (5 > ans)) {
    ans = 5;
    res.type_ = Type::KEY;
  }

  if (strStartWith("while") && (5 > ans)) {
    ans = 5;
    res.type_ = Type::KEY;
  }

  if (strStartWith("if") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::KEY;
  }

  if (strStartWith("else") && (4 > ans)) {
    ans = 4;
    res.type_ = Type::KEY;
  }

  if (strStartWith("break") && (5 > ans)) {
    ans = 5;
    res.type_ = Type::KEY;
  }

  if (((buffer_->at(cur_) >= 'a') && (buffer_->at(cur_) <= 'z')) ||
      ((buffer_->at(cur_) >= 'A') && (buffer_->at(cur_) <= 'Z')) ||
      (buffer_->at(cur_) == '_')) {
    size_t tail = cur_ + 1;
    while ((tail < buffer_->size()) &&
           (((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'z')) ||
            ((buffer_->at(tail) >= 'A') && (buffer_->at(tail) <= 'Z')) ||
            (buffer_->at(tail) == '_') ||
            ((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')))) {
      ++tail;
    }
    if (tail - cur_ > ans) {
      ans = tail - cur_;
      res.type_ = Type::IDENTIFIER;
    }
  }

  if ((buffer_->at(cur_) >= '0') && (buffer_->at(cur_) <= '9')) {
    int val = buffer_->at(cur_) - '0';
    size_t tail = cur_ + 1;
    while ((tail < buffer_->size()) &&
           ((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9'))) {
      val = val * 10 + buffer_->at(cur_) - '0';
      ++tail;
    }
    if (tail - cur_ > ans) {
      ans = tail - cur_;
      res.type_ = Type::INT_CONST;
      res.int_val_ = val;
    }
  }

  if (strStartWith("0x") || strStartWith("0X")) {
    int val = buffer_->at(cur_ + 2) - '0';
    size_t tail = cur_ + 3;
    while ((tail < buffer_->size()) &&
           (((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')) ||
            ((buffer_->at(tail) >= 'A') && (buffer_->at(tail) <= 'F')) ||
            ((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'f')))) {
      val = val * 16;
      if ((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')) {
        val += buffer_->at(tail) - '0';
      } else if ((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'f')) {
        val += buffer_->at(tail) - 'a' + 10;
      } else {
        val += buffer_->at(tail) - 'A' + 10;
      }
      ++tail;
    }

    if (tail - cur_ > ans) {
      ans = tail - cur_;
      res.type_ = Type::INT_CONST;
      res.int_val_ = val;
    }
  }

  if (buffer_->at(cur_) == '\'') {
    int tail = cur_ + 1;
    auto val_index = getConstChar(tail);
    tail = val_index.second;
    assert(buffer_->at(tail) == '\'');
    ++tail;
    if (tail - cur_ > ans) {
      ans = tail - cur_;
      res.type_ = Type::CHAR_CONST;
      res.char_val_ = val_index.first;
    }
  }

  if (buffer_->at(cur_) == '\"') {
    size_t tail = cur_ + 1;
    while ((tail < buffer_->size()) &&
           ((buffer_->at(tail) != '\"') || (buffer_->at(tail - 1) == '\\'))) {
      if ((buffer_->at(tail) == '\\') && (buffer_->at(tail + 1) == '\n')) {
        tail += 2;
        continue;
      }
      assert(buffer_->at(tail) != '\n');
      ++tail;
    }
    assert(buffer_->at(tail) == '\"');
    if (tail + 1 - cur_ > ans) {
      ans = tail - cur_ + 1;
      res.type_ = Type::STRING_CONST;
      std::string val = "";
      tail = cur_ + 1;
      while ((tail < buffer_->size()) &&
             ((buffer_->at(tail) != '\"') || (buffer_->at(tail - 1) == '\\'))) {
        if ((buffer_->at(tail) == '\\') && (buffer_->at(tail + 1) == '\n')) {
          tail += 2;
          continue;
        }

        auto val_index = getConstChar(tail);
        tail = val_index.second;
        val += std::string(1, val_index.first);
      }
      res.str_val_ = val;
    }
  }

  if ((buffer_->at(cur_) == '(') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == ')') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == ';') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == ',') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '=') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '{') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '}') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '[') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == ']') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '*') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '|') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '^') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '&') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '<') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '>') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '+') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '-') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '/') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '%') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '~') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '!') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if ((buffer_->at(cur_) == '.') && 1 > ans) {
    ans = 1;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("||") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("&&") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("==") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("!=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("<=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith(">=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("<<") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith(">>") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("++") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("--") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("->") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("+=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("-=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("*=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("/=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("%=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("|=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("^=") && (2 > ans)) {
    ans = 2;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith(">>=") && (2 > ans)) {
    ans = 3;
    res.type_ = Type::OPERATION;
  }

  if (strStartWith("<<=") && (2 > ans)) {
    ans = 3;
    res.type_ = Type::OPERATION;
  }

  assert(ans != 0);
  res.literal_ = "";
  for (size_t index = cur_; index - cur_ < ans;) {
    if ((res.type_ == Type::STRING_CONST) && (buffer_->at(index) == '\\') &&
        (buffer_->at(index + 1) == '\n')) {
      index += 2;
    }
    res.literal_ += std::string(1, buffer_->at(index));
    ++index;
  }

  // for debug
  //   std::cout << res.row_ << ", " << res.col_ << ": " << res.literal_
  //             << std::endl;
  jumpChars(ans);
  jumpUnuseChars();
  return res;
}

bool Lexer::strStartWith(const std::string& str) {
  if ((buffer_->size() - cur_) < str.size()) {
    return false;
  }

  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] != buffer_->at(cur_ + i)) {
      return false;
    }
  }
  return true;
}

std::pair<char, size_t> Lexer::getConstChar(size_t index) {
  //   char res = 0;
  if (buffer_->at(index) != '\\') {
    return {buffer_->at(index), index + 1};
  } else if (buffer_->at(index + 1) == 'a') {
    return {'\a', index + 2};
  } else if (buffer_->at(index + 1) == 'b') {
    return {'\b', index + 2};
  } else if (buffer_->at(index + 1) == 'f') {
    return {'\f', index + 2};
  } else if (buffer_->at(index + 1) == 'n') {
    return {'\n', index + 2};
  } else if (buffer_->at(index + 1) == 't') {
    return {'\t', index + 2};
  } else if (buffer_->at(index + 1) == 'v') {
    return {'\v', index + 2};
  } else if (buffer_->at(index + 1) == 'r') {
    return {'\r', index + 2};
  } else if (buffer_->at(index + 1) == '\"') {
    return {'\"', index + 2};
  } else if (buffer_->at(index + 1) == '\'') {
    return {'\'', index + 2};
  } else if (buffer_->at(index + 1) == '\\') {
    return {'\\', index + 2};
  } else if ((buffer_->at(index + 1) == 'X') ||
             (buffer_->at(index + 1) == 'x')) {
    size_t tail = index + 2;
    int val = 0;
    assert(((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')) ||
           ((buffer_->at(tail) >= 'A') && (buffer_->at(tail) <= 'F')) ||
           ((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'f')));

    while ((tail < buffer_->size()) &&
           (((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')) ||
            ((buffer_->at(tail) >= 'A') && (buffer_->at(tail) <= 'F')) ||
            ((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'f')))) {
      val = val * 16;
      if ((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '9')) {
        val += buffer_->at(tail) - '0';
      } else if ((buffer_->at(tail) >= 'a') && (buffer_->at(tail) <= 'f')) {
        val += buffer_->at(tail) - 'a' + 10;
      } else {
        val += buffer_->at(tail) - 'A' + 10;
      }
      ++tail;
    }
    return {val, tail};
  } else {
    size_t tail = index + 1;
    int val = 0;
    assert((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '7'));
    while ((buffer_->at(tail) >= '0') && (buffer_->at(tail) <= '7')) {
      val = val * 8 + buffer_->at(tail) - '0';
      ++tail;
    }
    return {val, tail};
  }
}