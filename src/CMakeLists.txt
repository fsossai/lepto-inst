add_llvm_pass_plugin(
  LeptoInst
  LeptoInstVisitor.cpp
  Pass.cpp
)

target_link_libraries(LeptoInst PRIVATE LLVMDemangle)
