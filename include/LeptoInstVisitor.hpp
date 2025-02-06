#include <regex>
#include <string>

#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"

class LeptoInstVisitor
    : public llvm::InstVisitor<LeptoInstVisitor, std::string> {
public:
  LeptoInstVisitor() = default;

  std::string operator()(llvm::Value &);
  std::string visitGetElementPtrInst(llvm::GetElementPtrInst &);
  std::string visitInstruction(llvm::Instruction &);
  std::string visitLoadInst(llvm::LoadInst &);
  std::string visitStoreInst(llvm::StoreInst &);
  std::string visitPHINode(llvm::PHINode &);
  std::string visitCallInst(llvm::CallInst &);
  std::string visitICmpInst(llvm::ICmpInst &);
  std::string visitBitCastInst(llvm::BitCastInst &);
  std::string visitBranchInst(llvm::BranchInst &);
  std::string visitValue(llvm::Value &);

private:
  std::string getId(llvm::Value *value);
};
