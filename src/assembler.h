#include <memory>
#include <sstream>
#include <vector>

#include "type.h"

#ifndef SRC_ASSEMBLER_
#define SRC_ASSEMBLER_

class Assembler {
 public:
  Assembler(std::shared_ptr<Environment> global,
            std::shared_ptr<Function> func_head);

 public:
  std::shared_ptr<std::string> getMips();

 private:
  void getMipsStatic();
  std::string realGlobal(const std::string& str);

 private:
  std::ostringstream oss_;
  std::shared_ptr<Environment> global_;
  std::shared_ptr<Function> func_head_;
  std::shared_ptr<std::string> mipss_;
};

#endif  // SRC_ASSEMBLER_