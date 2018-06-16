#include <memory>
#include <vector>

#include "type.h"

#ifndef SRC_ASSEMBLER_
#define SRC_ASSEMBLER_

class Assembler {
 public:
  Assembler(std::shared_ptr<Environment> global,
            std::shared_ptr<Function> func_head);

 public:
  std::shared_ptr<std::vector<std::string>> getMips();

 private:
  std::shared_ptr<Environment> global_;
  std::shared_ptr<Function> func_head_;
  std::shared_ptr<std::vector<std::string>> mipss_;
};

#endif  // SRC_ASSEMBLER_