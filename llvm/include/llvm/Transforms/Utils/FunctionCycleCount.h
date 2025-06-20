#ifndef LLVM_TRANSFORMS_UTILS_FUNCTIONCYCLECOUNT_H
#define LLVM_TRANSFORMS_UTILS_FUNCTIONCYCLECOUNT_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class Function;
class TargetTransformInfo;
class BasicBlock;
class Instruction;

class FunctionCycleCountPass : public PassInfoMixin<FunctionCycleCountPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);

private:
  unsigned calculateFunctionCycles(Function &F, TargetTransformInfo &TTI);
  unsigned calculateBasicBlockCycles(BasicBlock &BB, TargetTransformInfo &TTI);
  unsigned getInstructionCycles(Instruction &I, TargetTransformInfo &TTI);
};

} // namespace llvm

#endif