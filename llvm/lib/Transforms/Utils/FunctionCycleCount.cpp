#include "llvm/Transforms/Utils/FunctionCycleCount.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

PreservedAnalyses FunctionCycleCountPass::run(Module &M, ModuleAnalysisManager &AM) {
  for (Function &F : M) {
    errs() << "Analyzing function: " << F.getName() << "\n";
    // Skip declarations (functions without body)
    if (F.isDeclaration()) 
      continue;
    
    // Get TargetTransformInfo for this function
    FunctionAnalysisManager &FAM = 
      AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
    TargetTransformInfo &TTI = FAM.getResult<TargetIRAnalysis>(F);
    
    // Calculate cycles for this function
    unsigned cycles = calculateFunctionCycles(F, TTI);
    
    // Output the result
    errs() << "Function " << F.getName() << ": " << cycles << " cycles\n";
  }
  
  return PreservedAnalyses::all();
}

unsigned FunctionCycleCountPass::calculateFunctionCycles(Function &F, TargetTransformInfo &TTI) {
  unsigned totalCycles = 0;
  
  for (BasicBlock &BB : F) {
    totalCycles += calculateBasicBlockCycles(BB, TTI);
  }
  
  return totalCycles;
}

unsigned FunctionCycleCountPass::calculateBasicBlockCycles(BasicBlock &BB, TargetTransformInfo &TTI) {
  unsigned cycles = 0;
  
  for (Instruction &I : BB) {
    cycles += getInstructionCycles(I, TTI);
  }
  
  return cycles;
}

unsigned FunctionCycleCountPass::getInstructionCycles(Instruction &I, TargetTransformInfo &TTI) {
  if (auto *CI = dyn_cast<CallInst>(&I)) {
    return 5;
  }

  if (isa<LoadInst>(&I)) {
    TargetTransformInfo::TargetCostKind CostKind = TargetTransformInfo::TCK_Latency;
    return TTI.getMemoryOpCost(Instruction::Load, I.getType(), Align(1), 0, CostKind).getValue();
  }

  if (isa<StoreInst>(&I)) {
    TargetTransformInfo::TargetCostKind CostKind = TargetTransformInfo::TCK_Latency;
    return TTI.getMemoryOpCost(Instruction::Store, I.getType(), Align(1), 0, CostKind).getValue();
  }

  if (auto *BO = dyn_cast<BinaryOperator>(&I)) {
    TargetTransformInfo::TargetCostKind CostKind = TargetTransformInfo::TCK_Latency;
    return TTI.getArithmeticInstrCost(BO->getOpcode(), BO->getType(), CostKind).getValue();
  }

  return 1;
}
