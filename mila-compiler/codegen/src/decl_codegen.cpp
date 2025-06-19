#include <codegen/code_generator.h>
#include <exception/codegen_exception.h>
#include <llvm/IR/Verifier.h>
#include <iostream>

void CodeGenerator::visit(ConstDecl* decl) {
  decl->value()->accept(*this);
  _constants[decl->name()] = llvm::dyn_cast<llvm::Constant>(_value);;
}

void CodeGenerator::visit(VarDecl* decl) {
  auto name = decl->name();
  auto type = decl->type();
  
  auto currentFunction = _builder.GetInsertBlock() ? _builder.GetInsertBlock()->getParent() : nullptr;
  if (currentFunction) { // Local
    llvm::IRBuilder<> entryBuilder(&currentFunction->getEntryBlock(),
                                   currentFunction->getEntryBlock().begin());
    auto alloca = entryBuilder.CreateAlloca(getLLVMType(type), nullptr, name.c_str());
    auto init = getLLVMDefaultValue(type);
    _builder.CreateStore(init, alloca);
    _variables[name] = alloca;
  } else { // Global
    auto global = new llvm::GlobalVariable(*_module, getLLVMType(type), false,
                                           llvm::GlobalVariable::ExternalLinkage,
                                           getLLVMDefaultValue(type), name);
    _globals[name] = global;
  }
}

void CodeGenerator::visit(ArrayDecl* decl) {
  auto name = decl->name();
  auto type = decl->type();
  
  _arrayDecls[name] = decl; // Store array declaration for future access
  
  uint32_t size = decl->end() - decl->start() + 1;
  
  auto arrayType = llvm::ArrayType::get(getLLVMType(type), size);

  auto currentFunction = _builder.GetInsertBlock() ? _builder.GetInsertBlock()->getParent() : nullptr;
  if (currentFunction) { // Local
    llvm::IRBuilder<> entryBuilder(&currentFunction->getEntryBlock(),
                                   currentFunction->getEntryBlock().begin());
    auto alloca = entryBuilder.CreateAlloca(arrayType, nullptr, name.c_str());
    auto zero = getLLVMDefaultValue(type);
    for (uint32_t i = 0; i < size; i++) {
      auto idx = llvm::ConstantInt::get(_builder.getInt32Ty(), i);
      auto ptr = _builder.CreateInBoundsGEP(arrayType, alloca,
                                            { _builder.getInt32(0), idx});
      _builder.CreateStore(zero, ptr);
    }
    _variables[name] = alloca;
  } else { // Global
    auto init = llvm::ConstantAggregateZero::get(arrayType);
    auto array = new llvm::GlobalVariable(*_module, arrayType, false,
                                          llvm::GlobalVariable::ExternalLinkage,
                                          init, name);
    _globals[name] = array;
  }
}

void CodeGenerator::visit(FunctionDecl* decl) {
  // Function Signatures
  auto name = decl->name();
  auto type = decl->returnType();
  std::vector<llvm::Type*> argTypes;
  std::vector<std::string> argNames;
  for (auto &arg : decl->params()) {
    argTypes.push_back(getLLVMType(arg.type));
    argNames.push_back(arg.name);
  }
  auto retType = getLLVMType(type);
  auto functionType = llvm::FunctionType::get(retType, argTypes, false);
  
  auto function = _module->getFunction(name);
  
  if (function) {
    if (function->getFunctionType() != functionType) {
      throw CodeGenException(decl->location(), "Redefine of function: " + name);
    }
  } else {
    function = llvm::Function::Create(functionType,
                                      llvm::Function::ExternalLinkage, name,
                                      _module.get());
  }

  _functions[name] = function;

  if (!decl->body()) {
    // Forward declaration only
    return;
  }

  uint32_t i = 0;
  for (auto &arg : function->args())
    arg.setName(argNames[i++]);
  
  // Variables and Constants
  auto block = llvm::BasicBlock::Create(_context, "entry", function);
  _builder.SetInsertPoint(block);
  llvm::IRBuilder<> builder(&function->getEntryBlock(), function->getEntryBlock().begin());
  for (auto& arg : function->args()) {
    std::string arg_name = std::string(arg.getName());
    auto alloca = builder.CreateAlloca(arg.getType(), nullptr, arg_name.c_str());
    _builder.CreateStore(&arg, alloca);
    _variables[arg_name] = alloca;
  }
  
  auto consts = _constants;
  for (auto& const_ : decl->consts())
    const_->accept(*this);
  
  auto vars = _variables;
  for (auto& var : decl->vars())
    var->accept(*this);
  
  if (decl->returnType() != TOK_VOID)
    _variables[name] = builder.CreateAlloca(retType, nullptr, name.c_str());
  
  // Body
  auto retBlock = llvm::BasicBlock::Create(_context, "return", function);
//  _builder.SetInsertPoint(block);
  _returnBlock = retBlock;
  decl->body()->accept(*this);
  _builder.CreateBr(retBlock);
  _builder.SetInsertPoint(retBlock);
  
  auto retValue = type == TOK_VOID ? nullptr : _builder.CreateLoad(retType, _variables[name]);
  _builder.CreateRet(retValue);
  
  // Roll back context
  _constants = consts;
  _variables.clear();
}

void CodeGenerator::visit(ProgramDecl* program) {
  for (auto &const_ : program->consts())
    const_->accept(*this);

  for (auto &var : program->vars())
    var->accept(*this);

  for (auto &func : program->functions())
    func->accept(*this);

  auto mainFuncType = llvm::FunctionType::get(_builder.getInt32Ty(), false);
  auto mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", _module.get());
  auto entryBlock = llvm::BasicBlock::Create(_context, "entry", mainFunc);
  auto exitBlock = llvm::BasicBlock::Create(_context, "exit", mainFunc);

  _builder.SetInsertPoint(entryBlock);

  program->body()->accept(*this);
  
  _builder.CreateBr(exitBlock);
  _builder.SetInsertPoint(exitBlock);
  _builder.CreateRet(_builder.getInt32(0));

  if (llvm::verifyFunction(*mainFunc, &llvm::errs())) {
    throw CodeGenException(program->body()->location(), "Main function verification failed.");
  }
}
