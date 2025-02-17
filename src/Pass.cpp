#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "LeptoInstVisitor.hpp"

using namespace llvm;

struct LeptoInstPass : public FunctionPass {
  static char ID;

  LeptoInstPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    LeptoInstVisitor lepto;
    for (auto &BB : F) {
      for (auto &I : BB) {
        errs() << "Before: " << I << "\n";
        errs() << "After: " << lepto(I) << "\n\n";
      }
    }
    return false;
  }
};

char LeptoInstPass::ID = 0;
static RegisterPass<LeptoInstPass> X("LeptoInst", "Print shorter instructions");
