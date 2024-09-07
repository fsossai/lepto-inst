#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct LeptoInstPass : public FunctionPass {
  static char ID;

  LeptoInstPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    for (auto &BB : F) {
      for (auto &I : BB) {
        // TODO
      }
    }
    return false;
  }
};

} // namespace

char LeptoInstPass::ID = 0;
static RegisterPass<LeptoInstPass> X("LeptoInst",
                                     "Print shorter instructions");
