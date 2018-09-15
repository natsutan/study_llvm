#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"


static llvm::LLVMContext Context;
static llvm::Module *ModuleOb = new llvm::Module("My comupiler", Context);
static std::vector<std::string> FunArgs;
typedef llvm::SmallVector<llvm::BasicBlock *, 16> BBList;
typedef llvm::SmallVector<llvm::Value *, 16> ValList;


llvm::Function *createFunc(llvm::IRBuilder<> &Builder, std::string Name)
{
  //arg
  std::vector<llvm::Type *> Integers(FunArgs.size(), llvm::Type::getInt32Ty(Context));
  
  //return type
  llvm::FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);

  llvm::Function *fooFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
  return fooFunc;

}

void setFuncArgs(llvm::Function *fooFunc, std::vector<std::string> FunArgs)
{
  unsigned Idx = 0;
  llvm::Function::arg_iterator AI, AE;
  for(AI=fooFunc->arg_begin(), AE=fooFunc->arg_end(); AI != AE; ++AI, ++Idx)
    AI->setName(FunArgs[Idx]);
  
}

llvm::BasicBlock *createBB(llvm::Function *fooFunc, std::string Name)
{
  return llvm::BasicBlock::Create(Context, Name, fooFunc);
}

llvm::GlobalVariable *createGlob(llvm::IRBuilder<> &Builder, std::string Name)
{
  ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
  llvm::GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
  gVar->setLinkage(llvm::GlobalValue::CommonLinkage);
  gVar->setAlignment(4);
  return gVar;
}

llvm::Value *createArith(llvm::IRBuilder<> &Builder, llvm::Value *L, llvm::Value *R)
{
  return Builder.CreateMul(L, R, "multmp");
}

llvm::Value *createIfElse(llvm::IRBuilder<> &Builder, BBList List, ValList VL)
{
  llvm::Value *Condtn = VL[0];
  llvm::Value *Arg1 = VL[1];
  llvm::BasicBlock *ThenBB = List[0];
  llvm::BasicBlock *ElseBB = List[1];
  llvm::BasicBlock *MergeBB = List[2];
  Builder.CreateCondBr(Condtn, ThenBB, ElseBB);

  Builder.SetInsertPoint(ThenBB);
  llvm::Value *ThenVal = Builder.CreateAdd(Arg1, Builder.getInt32(1), "thenaddtmp");
  Builder.CreateBr(MergeBB);
  
  Builder.SetInsertPoint(ElseBB);
  llvm::Value *ElseVal = Builder.CreateAdd(Arg1, Builder.getInt32(2), "elsetmp");
  Builder.CreateBr(MergeBB);

  unsigned PhiBBSize = List.size() - 1;
  Builder.SetInsertPoint(MergeBB);
  llvm::PHINode *Phi = Builder.CreatePHI(llvm::Type::getInt32Ty(Context), PhiBBSize, "iftmp");
  Phi->addIncoming(ThenVal, ThenBB);
  Phi->addIncoming(ElseVal, ElseBB);
				  
  return Phi;
  
  
}


int main(int argc, char *argv[])
{
  FunArgs.push_back("a");
  FunArgs.push_back("b");
  
  static llvm::IRBuilder<> Builder(Context);
  llvm::GlobalVariable *gVar = createGlob(Builder, "x");
  llvm::Function *fooFunc = createFunc(Builder, "foo");
  setFuncArgs(fooFunc, FunArgs);
  llvm::BasicBlock *entry = createBB(fooFunc, "entry");
  Builder.SetInsertPoint(entry);
  llvm::Function::arg_iterator args = fooFunc->arg_begin();
  llvm::Value *Arg1 = &*args++;
  llvm::Value *constant = Builder.getInt32(16);
  llvm::Value *val = createArith(Builder, Arg1, constant);

  llvm::Value *val2 = Builder.getInt32(100);
  llvm::Value *Compare = Builder.CreateICmpULT(val, val2, "cmptmp");
  llvm::Value *Condtn = Builder.CreateICmpNE(Compare, Builder.getInt32(0));

  ValList VL;
  VL.push_back(Condtn);
  VL.push_back(Arg1);

  llvm::BasicBlock *ThenBB = createBB(fooFunc, "then");
  llvm::BasicBlock *ElseBB = createBB(fooFunc, "else");
  llvm::BasicBlock *MergeBB = createBB(fooFunc, "ifcont");
  BBList List;
  List.push_back(ThenBB);
  List.push_back(ElseBB);
  List.push_back(MergeBB);

  llvm::Value *v = createIfElse(Builder, List, VL);
  
  Builder.CreateRet(v);
  verifyFunction(*fooFunc);
  ModuleOb->dump();
  
  return 0;
  
}

