#include <codegen/code_generator.h>
#include <exception/codegen_exception.h>

void CodeGenerator::visit(IntegerLiteral* expr) {
  _value = llvm::ConstantInt::get(_builder.getInt32Ty(), expr->value());
}

void CodeGenerator::visit(FloatLiteral* expr) {
  _value = llvm::ConstantFP::get(_builder.getDoubleTy(), expr->value());
}

void CodeGenerator::visit(StringLiteral* expr) {
  auto str = expr->value();
  _value = _builder.CreateGlobalStringPtr(std::move(str), "string");
}

void CodeGenerator::visit(VariableExpr* expr) {
  std:: string name = expr->name();
  if (_constants[name]) {
    _value = _constants[name];
    return;
  }
  if (_variables[name]) {
    auto var = _variables[name];
    _value = _builder.CreateLoad(var->getAllocatedType(), var, name);
    return;
  }
  if (_globals[name]) {
    auto var = _globals[name];
    _value = _builder.CreateLoad(var->getValueType(), var, name);
    return;
  }
  throw CodeGenException(expr->location(), "Undefined variable: " + name);
}

void CodeGenerator::visit(BinaryExpr* expr) {
  expr->left()->accept(*this);
  auto left = _value;
  expr->right()->accept(*this);
  auto right = _value;
  
  auto doubleType = _builder.getDoubleTy();
  auto stringType = _builder.getPtrTy();

  if (left->getType() == stringType || right->getType() == stringType) {
    _value = get_bin_expr_str(left, right, expr->op()->type(), expr->op()->location());
  } else if (left->getType() == doubleType || right->getType() == doubleType) {
    _value = get_bin_expr_double(left, right, expr->op()->type(), expr->op()->location());
  } else {
    _value = get_bin_expr_int(left, right, expr->op()->type(), expr->op()->location());
  }
}

void CodeGenerator::visit(UnaryExpr* expr) {
  expr->expr()->accept(*this);
  auto operand = _value;
  
  switch(expr->op()->type()) {
    case TOK_PLUS:
      return;
    case TOK_MINUS:
      if (operand->getType()->isDoubleTy())
        _value = _builder.CreateFNeg(operand, "fneg");
      else
        _value = _builder.CreateNeg(operand, "neg");
      return;
    case TOK_NOT:
      if (operand->getType()->isIntegerTy(1)) {
        _value = _builder.CreateNot(operand, "not");
        return;
      } else {
        llvm::Value* zero;
        if (operand->getType()->isFloatingPointTy())
          zero = llvm::ConstantFP::get(operand->getType(), 0.0);
        else
          zero = llvm::ConstantInt::get(operand->getType(), 0);

        auto cmp = operand->getType()->isFloatingPointTy()
                   ? _builder.CreateFCmpONE(operand, zero)
                   : _builder.CreateICmpNE(operand, zero);

        _value = _builder.CreateNot(cmp, "not");
        return;
      }
    default:
      throw CodeGenException(expr->location(), "Unknown unary operator: " + expr->op()->type());
  }
}

void CodeGenerator::visit(CallExpr* expr) {
  _value = emitCall(expr->callee(), expr->args(), expr->location());
}

void CodeGenerator::visit(ParenExpr* expr) {
  expr->expr()->accept(*this);
}

void CodeGenerator::visit(ArrayAccess* expr) {
  auto name = expr->array();
  expr->index()->accept(*this);
  auto index = _value;
  
  llvm::Value* array;
  llvm::ArrayType* arrayType = nullptr;
  llvm::Type* elementType = nullptr;
  
  if ((array = _globals[name])) {
    auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(array);
    arrayType = llvm::cast<llvm::ArrayType>(gv->getValueType());
    elementType = arrayType->getElementType();
  } else if ((array = _variables[name])) {
    auto* alloc = llvm::dyn_cast<llvm::AllocaInst>(array);
    arrayType = llvm::cast<llvm::ArrayType>(alloc->getAllocatedType());
    elementType = arrayType->getElementType();
  } else {
    throw CodeGenException(expr->location(), "Not a valid array variable: " + name);
  }

  int startIndex = 0;
  
  auto it = _arrayDecls.find(name);
  if (it != _arrayDecls.end()) {
    startIndex = it->second->start();  // 선언 정보에서 시작 인덱스 얻기
  } else {
    throw CodeGenException(expr->location(), "No ArrayDecl found for: " + name);
  }
  
  if (startIndex != 0) {
    auto adj = llvm::ConstantInt::get(index->getType(), startIndex);
    index = _builder.CreateSub(index, adj, "index_adj");
  }
  
  auto gep = _builder.CreateInBoundsGEP(arrayType, array,
                                        { _builder.getInt32(0), index },
                                        "element_ptr");

  _value = _builder.CreateLoad(elementType, gep, "element");
}

llvm::Value* CodeGenerator::get_bin_expr_double(llvm::Value* left, llvm::Value* right, TokenType type, const SourceLocation loc) {
  if (left->getType() == _builder.getInt32Ty())
    left = _builder.CreateSIToFP(left, _builder.getDoubleTy());
  else if (right->getType() == _builder.getInt32Ty())
    right = _builder.CreateSIToFP(right, _builder.getDoubleTy());

  switch(type) {
    case TOK_PLUS:
      return _builder.CreateFAdd(left, right, "add");
    case TOK_MINUS:
      return _builder.CreateFSub(left, right, "sub");
    case TOK_MULTIPLY:
      return _builder.CreateFMul(left, right, "mul");
    case TOK_DIVIDE:
      return _builder.CreateFDiv(left, right, "divide");
//    case TOK_DIV:
//      return _builder.CreateSDiv(left, right, "div");
    case TOK_MOD:
      return _builder.CreateFRem(left, right, "mod");
    case TOK_LESS:
      return _builder.CreateFCmpOLT(left, right, "cmp");
    case TOK_LESS_OR_EQUAL:
      return _builder.CreateFCmpOLE(left, right, "cmp");
    case TOK_GREATER:
      return _builder.CreateFCmpOGT(left, right, "cmp");
    case TOK_GREATER_OR_EQUAL:
      return _builder.CreateFCmpOGE(left, right, "cmp");
    case TOK_EQUAL:
      return _builder.CreateFCmpOEQ(left, right, "cmp");
    case TOK_NOT_EQUAL:
      return _builder.CreateFCmpONE(left, right, "cmp");
//    case TOK_AND:
//      return _builder.CreateAnd(left, right, "cmp");
//    case TOK_OR:
//      return _builder.CreateOr(left, right, "cmp");
    default: throw CodeGenException(loc, "Not Implemented");
  }
}

llvm::Value* CodeGenerator::get_bin_expr_int(llvm::Value* left, llvm::Value* right, TokenType type, const SourceLocation loc) {
  switch(type) {
    case TOK_PLUS:
      return _builder.CreateAdd(left, right, "add");
    case TOK_MINUS:
      return _builder.CreateSub(left, right, "sub");
    case TOK_MULTIPLY:
      return _builder.CreateMul(left, right, "mul");
    case TOK_DIVIDE:
    case TOK_DIV:
      return _builder.CreateSDiv(left, right, "div");
    case TOK_MOD:
      return _builder.CreateSRem(left, right, "mod");
    case TOK_LESS:
      return _builder.CreateICmpSLT(left, right, "cmp");
    case TOK_LESS_OR_EQUAL:
      return _builder.CreateICmpSLE(left, right, "cmp");
    case TOK_GREATER:
      return _builder.CreateICmpSGT(left, right, "cmp");
    case TOK_GREATER_OR_EQUAL:
      return _builder.CreateICmpSGE(left, right, "cmp");
    case TOK_EQUAL:
      return _builder.CreateICmpEQ(left, right, "cmp");
    case TOK_NOT_EQUAL:
      return _builder.CreateICmpNE(left, right, "cmp");
    case TOK_AND:
      return _builder.CreateAnd(left, right, "cmp");
    case TOK_OR:
      return _builder.CreateOr(left, right, "cmp");
    default: throw CodeGenException(loc, "Not Implemented");
  }
}

llvm::Value* CodeGenerator::get_bin_expr_str(llvm::Value* left, llvm::Value* right, TokenType type, const SourceLocation loc) {
  if (type != TOK_PLUS)
    throw CodeGenException(loc, "String only supprot '+' operation");
  
  if (!left->getType()->isPointerTy())
    left = emitToString(left, left->getType());

  if (!right->getType()->isPointerTy())
    right = emitToString(right, right->getType());
  
  auto buf = _builder.CreateAlloca(llvm::ArrayType::get(_builder.getInt8Ty(), 128), nullptr, "concatbuf");
  auto bufPtr = _builder.CreatePointerCast(buf, _builder.getPtrTy(), "concatptr");
  
  llvm::Function* sprintf = _module->getFunction("sprintf");
  llvm::Function* strcat = _module->getFunction("strcat");

  auto fmtStr = _builder.CreateGlobalStringPtr("%s", "fmt");
  _builder.CreateCall(sprintf, { bufPtr, fmtStr, left }); // sprintf(buf, "%s", left)
  _builder.CreateCall(strcat, { bufPtr, right }); // strcat(buf, right)

  return bufPtr;
}
