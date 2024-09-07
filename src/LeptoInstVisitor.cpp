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

  if (isa<Instruction>(value) || isa<Argument>(value)) {
    regex pattern(R"([ ]+(%[^ ]+))");
    smatch match;
    if (regex_search(buffer, match, pattern)) {
      return match[1].str();
    }
  }
  return buffer;
}

string LeptoInstVisitor::visitInstruction(Instruction &I) { return "inst"; }

string LeptoInstVisitor::visitLoadInst(LoadInst &LI) {
  string output;
  output += getId(&LI);
  output += " = load";
  output += " " + getId(LI.getPointerOperand());
  return output;
}

string LeptoInstVisitor::visitStoreInst(StoreInst &SI) {
  string output = "store";
  output += " " + getId(SI.getValueOperand());
  output += " to " + getId(SI.getPointerOperand());
  return output;
}

