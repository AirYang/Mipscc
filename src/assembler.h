#include <memory>
#include <sstream>
#include <unordered_map>
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
  void optimization();

  void getMipsStatic();
  void getMipsBlock(std::shared_ptr<Block> blck);
  void getMipsInstruction(std::shared_ptr<Instruction> ins);

  int paraWidth(std::shared_ptr<ReturnType> th);
  int fitInWord(std::shared_ptr<ReturnType> th);
  std::string realGlobal(const std::string& str);

  void insLoadValue(const std::string& reg, std::shared_ptr<ReturnType> th);
  void insStoreBack(std::shared_ptr<ReturnType> th, const std::string& reg);
  void insLoadAddr(const std::string& reg, std::shared_ptr<ReturnType> th);

 private:
  std::ostringstream oss_;
  std::shared_ptr<Environment> global_;
  std::shared_ptr<Function> func_head_;
  std::shared_ptr<std::string> mipss_;
  std::vector<Register> regs_;
  int para_offset_;
  int bfs_cnt_;

  static const std::unordered_map<Type, std::string, EnumClassHash>
      unary_op_literal_;
  static const std::unordered_map<Type, std::string, EnumClassHash>
      unary_op_ins_;
  static const std::unordered_map<Type, std::string, EnumClassHash>
      binary_op_literal_;
  static const std::unordered_map<Type, std::string, EnumClassHash>
      binary_op_ins_;
  static const std::unordered_map<Type, std::string, EnumClassHash>
      btf_op_literal_;
  static const std::unordered_map<Type, std::string, EnumClassHash> btf_op_ins_;
  static const std::unordered_map<Type, std::string, EnumClassHash>
      bb_op_literal_;
  static const std::unordered_map<Type, std::string, EnumClassHash> bb_op_ins_;
};

#endif  // SRC_ASSEMBLER_