#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "LeptoInstVisitor.hpp"

using namespace std;
using namespace llvm;

string LeptoInstVisitor::getId(Value *value) {
  string buffer;
  raw_string_ostream stream(buffer);
  value->print(stream);
  stream.flush();

  if (auto CI = dyn_cast<ConstantInt>(value)) {
    return to_string(CI->getSExtValue());
  }

  if (isa<Instruction>(value) || isa<Argument>(value)) {
    regex pattern(R"([ ]+(%[^ ]+))");
    smatch match;
    if (regex_search(buffer, match, pattern)) {
      return match[1].str();
    }
  }
  return buffer;
}

string LeptoInstVisitor::visitGetElementPtrInst(GetElementPtrInst &GEP) {
  string output;
  output += getId(&GEP) + " = gep";
  output += " " + getId(GEP.getPointerOperand());
  return output;
}

string LeptoInstVisitor::visitInstruction(Instruction &I) { return "inst"; }

string LeptoInstVisitor::visitLoadInst(LoadInst &LI) {
  string output;
  output += getId(&LI) + " = load";
  output += " " + getId(LI.getPointerOperand());
  return output;
}

string LeptoInstVisitor::visitStoreInst(StoreInst &SI) {
  string output = "store";
  output += " " + getId(SI.getValueOperand());
  output += " to " + getId(SI.getPointerOperand());
  return output;
}

string LeptoInstVisitor::visitPHINode(PHINode &PHI) {
  string output;
  output += getId(&PHI) + " = phi ";
  for (uint32_t i = 0; i < PHI.getNumIncomingValues(); i++) {
    auto V = PHI.getIncomingValue(i);
    output += getId(V);
    if ((i + 1) < PHI.getNumIncomingValues()) {
      output += ", ";
    }
  }
  return output;
}
