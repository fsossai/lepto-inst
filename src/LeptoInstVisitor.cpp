#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "LeptoInstVisitor.hpp"

using namespace std;
using namespace llvm;

string LeptoInstVisitor::visitStoreInst(StoreInst &SI) {
  return "store";
}

string LeptoInstVisitor::visitInstruction(Instruction &I) {
  return "inst";
}
