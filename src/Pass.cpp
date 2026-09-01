#include "llvm/Pass.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#if LLVM_VERSION_MAJOR >= 22
#include "llvm/Plugins/PassPlugin.h"
#else
#include "llvm/Passes/PassPlugin.h"
#endif
#include "llvm/Support/raw_ostream.h"

#include "LeptoInstVisitor.hpp"

using namespace llvm;

namespace {

void printFunction(Function &F) {
  LeptoInstVisitor lepto;
  for (auto &BB : F) {
    for (auto &I : BB) {
      errs() << "Before : " << I << "\n";
      errs() << "After  : " << lepto(I) << "\n\n";
    }
  }
}

struct LeptoInstPass : PassInfoMixin<LeptoInstPass> {
  static bool isRequired() { return true; }

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    printFunction(F);
    return PreservedAnalyses::all();
  }
};

struct LegacyLeptoInstPass : public FunctionPass {
  static char ID;

  LegacyLeptoInstPass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    printFunction(F);
    return false;
  }
};

char LegacyLeptoInstPass::ID = 0;
static RegisterPass<LegacyLeptoInstPass> X("LeptoInst",
                                           "Print shorter instructions");

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LeptoInst", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name != "lepto-inst") {
                    return false;
                  }
                  FPM.addPass(LeptoInstPass());
                  return true;
                });
          }};
}
