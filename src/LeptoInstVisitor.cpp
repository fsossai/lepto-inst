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

bool fetchConstantString(Value *value, string &result) {
  auto GV = dyn_cast<llvm::GlobalVariable>(value);
  if (GV == nullptr || !GV->isConstant()) {
    return false;
  }
  auto ATy = dyn_cast<llvm::ArrayType>(GV->getValueType());
  if (ATy == nullptr) {
    return false;
  }
  if (!ATy->getElementType()->isIntegerTy(8)) {
    return false;
  }
  auto CDA = dyn_cast<ConstantDataArray>(GV->getInitializer());
  if (!CDA->isString()) {
    return false;
  }

  string tmp = CDA->getAsCString().str();
  const int MAX_LENGTH = 30;
  if (tmp.size() > MAX_LENGTH) {
    result = "\"" + tmp.substr(0, MAX_LENGTH - 3) + "...\"";
  } else {
    result = "\"" + tmp + "\"";
  }
  return true;
}

string LeptoInstVisitor::getId(Value *value) {
  string buffer;

  if (auto F = dyn_cast<Function>(value)) {
    return "@" + F->getName().str();
  }

  if (auto CI = dyn_cast<ConstantInt>(value)) {
    return to_string(CI->getSExtValue());
  }

  if (fetchConstantString(value, buffer)) {
    return buffer;
  }

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

string LeptoInstVisitor::visitGetElementPtrInst(GetElementPtrInst &GEP) {
  string output;
  output += getId(&GEP) + " = gep " + getId(GEP.getPointerOperand());

  for (auto idx = GEP.idx_begin(); idx != GEP.idx_end(); ++idx) {
    output += ", ";
    output += getId(*idx);
  }
  return output;
}

string LeptoInstVisitor::visitInstruction(Instruction &I) {
  string output;
  raw_string_ostream stream(output);
  I.print(stream);
  stream.flush();

  // remove white spaces at the beginning
  int ws = -1;
  while (output[++ws] == ' ')
    ;
  output = output.substr(ws, output.size() - ws);
  return output;
}

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

string LeptoInstVisitor::visitCallInst(CallInst &CI) {
  string output;
  if (!CI.getCalledFunction()) {
    return "call";
  }
  auto &F = *CI.getCalledFunction();
  if (!F.getReturnType()->isVoidTy()) {
    output += getId(&CI) += " = ";
  }

  string displayName = demangleName(F.getName().str());

  // removing trailing `const`
  displayName = regex_replace(displayName, regex(" const$"), "");
  // removing arguments
  displayName = regex_replace(displayName, regex("([(][^(]*[)]$)"), "");

  // removing <templates>
  string oldDisplayName;
  do {
    oldDisplayName = displayName;
    displayName = regex_replace(oldDisplayName, regex("(<.+>)"), "");
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

string LeptoInstVisitor::visitICmpInst(ICmpInst &I) {
  string output;
  output += getId(&I) + " = icmp ";
  output += getId(I.getOperand(0)) + ", ";
  output += getId(I.getOperand(1));
  return output;
}

string LeptoInstVisitor::visitBitCastInst(BitCastInst &BC) {
  string output;
  output += getId(&BC) += " = bitcast " + getId(BC.getOperand(0));
  return output;
}

string LeptoInstVisitor::visitBranchInst(BranchInst &BI) {
  string output;
  output += "br";
  if (BI.isConditional()) {
    output += " " + getId(BI.getCondition());
  }
  return output;
}

string LeptoInstVisitor::visitValue(Value &V) {
  if (auto I = dyn_cast<Instruction>(&V)) {
    return this->visit(I);
  }
  string output;
  raw_string_ostream stream(output);
  V.print(stream);
  stream.flush();
  return output;
}
