#include <regex>
#include <string>

#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"

class LeptoInstVisitor
    : public llvm::InstVisitor<LeptoInstVisitor, std::string> {
public:
  std::string visitGetElementPtrInst(llvm::GetElementPtrInst &);
  std::string visitInstruction(llvm::Instruction &);
  std::string visitLoadInst(llvm::LoadInst &);
  std::string visitStoreInst(llvm::StoreInst &);

private:
  std::string getId(llvm::Value *value);
};
