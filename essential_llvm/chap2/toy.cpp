#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"


static llvm::LLVMContext Context;
static llvm::Module *ModuleOb = new llvm::Module("My comupiler", Context);
static std::vector<std::string> FunArgs;

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
  Builder.CreateRet(Builder.getInt32(0));
  verifyFunction(*fooFunc);
  ModuleOb->dump();
  
  return 0;
  
}
