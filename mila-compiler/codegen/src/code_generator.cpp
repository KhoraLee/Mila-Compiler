#include <codegen/code_generator.h>

#include <exception/codegen_exception.h>

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
    case TOK_STRING:
      return llvm::ConstantPointerNull::get(_builder.getPtrTy());
    default:
      throw CodeGenException({-1, -1}, "Unknown type token");
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
  
    // sprintf
    std::vector<llvm::Type*> sprintf_args;
    sprintf_args.push_back(_builder.getPtrTy()); // buffer
    sprintf_args.push_back(_builder.getPtrTy()); // format
    llvm::FunctionType* sprintf_type = llvm::FunctionType::get(
        _builder.getInt32Ty(), sprintf_args, true);
    llvm::Function::Create(sprintf_type, llvm::Function::ExternalLinkage,
                          "sprintf", _module.get());

    // strcat
    std::vector<llvm::Type*> strcat_args;
    strcat_args.push_back(_builder.getPtrTy()); // dest
    strcat_args.push_back(_builder.getPtrTy()); // src
    llvm::FunctionType* strcat_type = llvm::FunctionType::get(
        _builder.getPtrTy(), strcat_args, false);
    llvm::Function::Create(strcat_type, llvm::Function::ExternalLinkage,
                          "strcat", _module.get());
}

llvm::Value* CodeGenerator::emitCall(const std::string& callee, const std::vector<Expr>& args, const SourceLocation loc) {
  // Built-in functions
  if (callee == "write" || callee == "writeln" || callee == "readln") {
    llvm::Function* builtin;
    std::string format;
    
    if (args.size() != 1) {
      throw CodeGenException(loc, callee + " functions should have only one arg");
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
        throw CodeGenException(loc, "Unsupported type for " + callee);
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
        throw CodeGenException(loc, "Unsupported type for " + callee);
      }

      fmtStr = _builder.CreateGlobalStringPtr(format, "fmt");

      return _builder.CreateCall(builtin, { fmtStr, arg }, "call_read");
    }
  } else if (callee == "inc" || callee == "dec") {
    if (args.size() < 1 || args.size() > 2) {
      throw CodeGenException(loc, callee + "expects 1 or 2 arguments");
    }

    auto varExpr = std::dynamic_pointer_cast<VariableExpr>(args.front());
    std::string varName = varExpr->name();
    llvm::Value* ptr = nullptr;

    if (_variables[varName]) {
      ptr = _variables[varName];
    } else if (_globals[varName]) {
      ptr = _globals[varName];
    } else {
      throw CodeGenException(loc, "Variable not found: " + varName);
    }

    llvm::Value* currentVal = _builder.CreateLoad(_builder.getInt32Ty(), ptr, varName);

    llvm::Value* step;
    if (args.size() == 2) {
      args[1]->accept(*this);
      step = _value;
    } else {
      step = llvm::ConstantInt::get(_builder.getInt32Ty(), 1);
    }

    llvm::Value* result;
    if (callee == "inc")
      result = _builder.CreateAdd(currentVal, step, "inc");
    else
      result = _builder.CreateSub(currentVal, step, "dec");

    _builder.CreateStore(result, ptr);
    return result;
  } else if (callee == "int" || callee == "float") {
    if (args.size() != 1) {
      throw CodeGenException(loc, callee + "expects 1 argument");
    }
    
    args.front()->accept(*this);
    auto arg = _value;
    
    if (callee == "int") {
      if (arg->getType()->isDoubleTy()) {
        return _builder.CreateFPToSI(arg, _builder.getInt32Ty());
      } else if (arg->getType()->isIntegerTy()) {
        return arg;
      } else {
        throw CodeGenException(loc, "Unsupported type for int()");
      }
    } else {
      if (arg->getType()->isDoubleTy()) {
        return arg;
      } else if (arg->getType()->isIntegerTy()) {
        return _builder.CreateSIToFP(arg, _builder.getDoubleTy());
      } else {
        throw CodeGenException(loc, "Unsupported type for float()");
      }
    }
  }

  // Functions declared in the code
  auto function = _module->getFunction(callee);
  if (!function) {
    throw CodeGenException(loc, "Function is not defined: " + callee);
  }

  // Check arg size
  if (args.size() != function->arg_size()) {
    throw CodeGenException(loc, "Arg size misamtch: " + callee);
  }
  
  std::vector<llvm::Value*> fun_args;
  for (const auto& arg : args) {
    arg->accept(*this);
    fun_args.push_back(_value);
  }
  
  auto isVoid = function->getReturnType()->isVoidTy();
  return _builder.CreateCall(function, fun_args, isVoid ? "" : "call_" + callee);
}

llvm::Value* CodeGenerator::emitToString(llvm::Value* value, llvm::Type* type) {
  auto buf = _builder.CreateAlloca(llvm::ArrayType::get(_builder.getInt8Ty(), 32), nullptr, "strbuf");
  auto bufPtr = _builder.CreatePointerCast(buf, _builder.getPtrTy(), "bufptr");

  llvm::Function* sprintfFn = _module->getFunction("sprintf");

  llvm::Value* fmtStr = nullptr;
  if (type->isDoubleTy()) {
    fmtStr = _builder.CreateGlobalStringPtr("%f", "fmt");
  } else {
    fmtStr = _builder.CreateGlobalStringPtr("%d", "fmt");
  }

  _builder.CreateCall(sprintfFn, { bufPtr, fmtStr, value });

  return bufPtr;
}
