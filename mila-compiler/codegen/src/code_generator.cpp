#include <codegen/code_generator.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>

CodeGenerator::CodeGenerator()
  : _builder(_context) {
  _module = std::make_unique<llvm::Module>("mila", _context);
  initializeBuiltinFunctions();
}

llvm::Type* CodeGenerator::getLLVMType(const TokenType type) {
  switch (type) {
    case TOK_INTEGER:
      return _builder.getInt32Ty();
    case TOK_FLOAT:
      return _builder.getDoubleTy();
    case TOK_STRING:
      return _builder.getPtrTy();
    default:
      return _builder.getVoidTy();
  }
}

llvm::Constant* CodeGenerator::getLLVMDefaultValue(TokenType type) {
  switch (type) {
    case TOK_INTEGER:
      return _builder.getInt32(0);
    case TOK_FLOAT:
      return llvm::ConstantFP::get(llvm::Type::getDoubleTy(_context), 0.0);
    default:
      return nullptr;
  }
}

void CodeGenerator::initializeBuiltinFunctions() {
    // printf
    std::vector<llvm::Type*> printf_args;
    printf_args.push_back(_builder.getPtrTy());
    llvm::FunctionType* printf_type = llvm::FunctionType::get(
        _builder.getInt32Ty(), printf_args, true);
    llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage,
                          "printf", _module.get());

    // scanf
    std::vector<llvm::Type*> scanf_args;
    scanf_args.push_back(_builder.getPtrTy());
    llvm::FunctionType* scanf_type = llvm::FunctionType::get(
        _builder.getInt32Ty(), scanf_args, true);
    llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage,
                          "scanf", _module.get());
}

llvm::Value* CodeGenerator::emitCall(const std::string& callee, const std::vector<Expr>& args) {
  // Built-in functions
  if (callee == "write" || callee == "writeln" || callee == "readln") {
    llvm::Function* builtin;
    std::string format;
    
    if (args.size() != 1) {
      llvm::errs() << "Built-in functions should have only one arg\n";
      // throw error
    }
    
    args.front()->accept(*this);
    llvm::Value* arg = _value;
    llvm::Value* fmtStr = nullptr;

    llvm::Type* argType = arg->getType();

    if (callee == "write" || callee == "writeln") {
      builtin = _module->getFunction("printf");

      if (argType->isIntegerTy(32)) {
        format = "%d";
      } else if (argType->isDoubleTy()) {
        format = "%f";
      } else if (argType->isPointerTy()) {
        format = "%s";
      } else {
        llvm::errs() << "Unsupported type for write/writeln\n";
        return nullptr;
      }

      if (callee == "writeln")
        format += "\n";

      fmtStr = _builder.CreateGlobalStringPtr(format, "fmt");
      return _builder.CreateCall(builtin, { fmtStr, arg }, "call_write");
    } else if (callee == "readln") {
      builtin = _module->getFunction("scanf");
      
      auto arg_expr = std::dynamic_pointer_cast<VariableExpr>(args.front());
      auto arg_name = arg_expr->name();
      
      if (_variables[arg_name])
        arg = _variables[arg_name];
      else if (_globals[arg_name])
        arg = _globals[arg_name];

      if (argType->isIntegerTy(32)) {
        format = "%d";
      } else if (argType->isDoubleTy()) {
        format = "%lf";
      } else {
        llvm::errs() << "Unsupported type for readln\n";
        return nullptr;
      }

      fmtStr = _builder.CreateGlobalStringPtr(format, "fmt");

      return _builder.CreateCall(builtin, { fmtStr, arg }, "call_read");
    }
  }

  // Functions declared in the code
  auto function = _module->getFunction(callee);
  if (!function) {
    llvm::errs() << "Function is not defined: '" << callee << "'\n";
    return; // throw error
  }

  // Check arg size
  if (args.size() != function->arg_size()) {
    llvm::errs() << "Arg size misamtch: '" << callee << "'\n";
    return; // throw error
  }
  
  std::vector<llvm::Value*> fun_args;
  for (const auto& arg : args) {
    arg->accept(*this);
    fun_args.push_back(_value);
  }
  
  return _builder.CreateCall(function, fun_args, "call_" + callee);
}
