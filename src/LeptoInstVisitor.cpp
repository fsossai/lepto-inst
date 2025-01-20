#include <cxxabi.h>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "LeptoInstVisitor.hpp"

using namespace std;
using namespace llvm;

string demangleName(const std::string &mangledName) {
  int status = 0;
  char *demangled =
      abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

  if (status == 0 && demangled) {
    string result(demangled);
    free(demangled);
    return result;
  } else {
    return mangledName;
  }
}

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

string LeptoInstVisitor::visitCall(CallInst &CI) {
  string output;
  if (!CI.getFunction()) {
    return "call";
  }
  auto &F = *CI.getFunction();
  if (!F.getReturnType()->isVoidTy()) {
    output += getId(&CI) += " = ";
  }

  string fName = demangleName(F.getName().str());

  // removing arguments
  regex pattern("([^(]+)");
  smatch match;
  string displayName;
  if (regex_search(fName, match, pattern)) {
    displayName = match[0].str();
  }

  // removing <templates>
  pattern = "(<.+>)";
  string oldDisplayName;
  do {
    oldDisplayName = displayName;
    displayName = regex_replace(oldDisplayName, pattern, "");
  } while (oldDisplayName != displayName);
  output += "call " + displayName + " (";

  for (uint32_t i = 0; i < CI.arg_size(); i++) {
    output += getId(CI.getArgOperand(i));
    if ((i + 1) < CI.arg_size()) {
      output += ", ";
    }
  }
  output += ")";
  return output;
}
