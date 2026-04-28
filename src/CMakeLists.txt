add_llvm_library(
  LeptoInst
  MODULE
  LeptoInstVisitor.cpp
  Pass.cpp
)

target_link_libraries(LeptoInst PRIVATE LLVMDemangle)
