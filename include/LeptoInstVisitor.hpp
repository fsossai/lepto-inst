#include <string>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstVisitor.h"

class LeptoInstVisitor : public llvm::InstVisitor<LeptoInstVisitor, std::string> {
public:
  std::string visitInstruction(llvm::Instruction &I);
  std::string visitStoreInst(llvm::StoreInst &SI);
};

