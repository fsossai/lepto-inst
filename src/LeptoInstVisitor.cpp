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

auto demangleName(string mangledName) -> string;
auto detemplate(string s) -> string;
auto fetchConstantString(Value *value, string &result) -> bool;
auto getId(Value *value) -> string;
auto getTypeStr(Type *type) -> string;
auto shortnenFunctionName(string name) -> string;
template <typename T> auto toString(T *a) -> string;
auto unescape(string s) -> string;

string LeptoInstVisitor::operator()(Value &V) { return this->visitValue(V); }

string LeptoInstVisitor::visitAllocaInst(AllocaInst &AI) {
  return getId(&AI) + " = alloca " + getTypeStr(AI.getAllocatedType());
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

string LeptoInstVisitor::visitInvokeInst(InvokeInst &II) {
  string output;
  if (!II.getCalledFunction()) {
    return "invoke";
  }
  auto &F = *II.getCalledFunction();
  if (!F.getReturnType()->isVoidTy()) {
    output += getId(&II) += " = ";
  }

  output += "invoke " + shortnenFunctionName(F.getName().str()) + " (";

  for (uint32_t i = 0; i < II.arg_size(); i++) {
    output += getId(II.getArgOperand(i));
    if ((i + 1) < II.arg_size()) {
      output += ", ";
    }
  }
  output += ")";
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

string LeptoInstVisitor::visitReturnInst(ReturnInst &RI) {
  string output = "ret ";
  if (RI.getNumOperands() > 0) {
    output += getId(RI.getOperandUse(0));
  } else {
    output += "void";
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

  output += "call " + shortnenFunctionName(F.getName().str()) + " (";

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
  } else if (auto A = dyn_cast<Argument>(&V)) {
    return this->visitArgument(*A);
  }
  string output;
  raw_string_ostream stream(output);
  V.print(stream);
  stream.flush();
  return output;
}

string LeptoInstVisitor::visitArgument(Argument &A) {
  return "arg " + getId(&A) + " " + getTypeStr(A.getType());
}

string LeptoInstVisitor::visitBinaryOperator(BinaryOperator &BO) {
  string op0 = getId(BO.getOperandUse(0));
  string op1 = getId(BO.getOperandUse(1));
  string output = getId(&BO) + " = ";

  string buffer = toString(&BO);

  regex pattern(R"([^=]+= ([a-z]+))");
  smatch match;
  if (regex_search(buffer, match, pattern)) {
    return output + match[1].str() + " " + op0 + ", " + op1;
  }

  return buffer;
}

string getId(Value *value) {
  string buffer;

  if (auto F = dyn_cast<Function>(value)) {
    return "@" + shortnenFunctionName(F->getName().str());
  }
  if (fetchConstantString(value, buffer)) {
    return buffer;
  }
  if (auto CI = dyn_cast<ConstantInt>(value)) {
    return to_string(CI->getSExtValue());
  }
  if (auto CFP = dyn_cast<ConstantFP>(value)) {
    return to_string(CFP->getValue().convertToDouble());
  }
  if (auto GV = dyn_cast<GlobalValue>(value)) {
    return "@" + demangleName(GV->getName().str());
  }

  buffer = toString(value);

  if (isa<Instruction>(value) || isa<Argument>(value)) {
    regex pattern(R"([ ]+(%[^ ]+))");
    smatch match;
    if (regex_search(buffer, match, pattern)) {
      return match[1].str();
    }
  }
  return buffer;
}

string getTypeStr(Type *type) {
  if (auto ST = dyn_cast<StructType>(type)) {
    if (ST->hasName()) {
      return detemplate(ST->getName().str());
    } else {
      return "<unnamed>";
    }
  }

  string buffer;
  raw_string_ostream stream(buffer);
  type->print(stream);
  stream.flush();

  return detemplate(buffer);
}

string detemplate(string s) {
  string prev_s;
  do {
    prev_s = s;
    s = regex_replace(prev_s, regex("(<.+>)"), "");
  } while (prev_s != s);
  return s;
}

string demangleName(string mangledName) {
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

  string raw = unescape(CDA->getAsCString().str());
  const int MAX_LENGTH = 30;
  if (raw.size() > MAX_LENGTH) {
    raw = raw.substr(0, MAX_LENGTH - 3) + "...";
  }
  result = "\"" + raw + "\"";
  return true;
}

string shortnenFunctionName(string name) {
  name = demangleName(name);

  // removing `const` specifier
  name = regex_replace(name, regex(" const"), "");
  // removing arguments
  name = regex_replace(name, regex("([(][^(]*[)]$)"), "");

  // removing <templates>
  name = detemplate(name);

  return name;
}

template <typename T> string toString(T *t) {
  string buffer;
  raw_string_ostream stream(buffer);
  t->print(stream);
  stream.flush();
  return buffer;
}

string unescape(string s) {
  string output;
  for (char c : s) {
    switch (c) {
    case '\\':
      output += "\\\\";
      break;
    case '\"':
      output += "\\\"";
      break;
    case '\r':
      output += "\\r";
      break;
    case '\n':
      output += "\\n";
      break;
    case '\f':
      output += "\\f";
      break;
    case '\t':
      output += "\\t";
      break;
    case '\v':
      output += "\\v";
      break;
    default:
      output += c;
    }
  }
  return output;
}
