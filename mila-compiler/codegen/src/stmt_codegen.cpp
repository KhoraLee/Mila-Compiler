#include <codegen/code_generator.h>
#include <exception/codegen_exception.h>

void CodeGenerator::visit(AssignStmt* stmt) {
  llvm::Value* var;
  
  auto name = stmt->target();
  stmt->value()->accept(*this);
  
  if ((var = _variables[name]) || (var = _globals[name])) {
    llvm::Type* targetType = nullptr;

    if (auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(var)) {
      targetType = gv->getValueType();
    } else if (auto* alloc = llvm::dyn_cast<llvm::AllocaInst>(var)) {
      targetType = alloc->getAllocatedType();
    } else {
      throw CodeGenException(stmt->location(), "Unsupported assignment target");
    }
    
    llvm::Value* valueToStore = _value;

    if (_value->getType() != targetType) {
      if (targetType->isDoubleTy() && _value->getType()->isIntegerTy()) {
        valueToStore = _builder.CreateSIToFP(_value, targetType, "cast_double");
      } else if (targetType->isIntegerTy() && _value->getType()->isDoubleTy()) {
        valueToStore = _builder.CreateFPToSI(_value, targetType, "cast_int");
      } else {
        std::string errStr;
        llvm::raw_string_ostream errStream(errStr);
        errStream << "Unsupported type cast from " << *_value->getType()
                  << " to " << *targetType;
        throw CodeGenException(stmt->location(), errStream.str());
      }
    }
    _builder.CreateStore(valueToStore, var);
  }
  if (_constants[name])
    throw CodeGenException(stmt->location(), "Cannot assgin value to constant");
  throw CodeGenException(stmt->location(), "Undefined variable: " + name);
}

void CodeGenerator::visit(ArrayAssignStmt* stmt) {
  const auto name = stmt->target();
  
  stmt->index()->accept(*this);
  auto index = _value;
  
  int startIndex = 0;
  
  auto it = _arrayDecls.find(name);
  if (it != _arrayDecls.end()) {
    startIndex = it->second->start();
  } else {
    throw CodeGenException(stmt->location(), "Undefined variable: " + name);
  }
  
  if (startIndex != 0) {
    auto adj = llvm::ConstantInt::get(index->getType(), startIndex);
    index = _builder.CreateSub(index, adj, "index_adj");
  }
  
  llvm::Value* array;
  llvm::ArrayType* arrayType = nullptr;
  
  if ((array = _globals[name])) {
    auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(array);
    arrayType = llvm::cast<llvm::ArrayType>(gv->getValueType());
  } else if ((array = _variables[name])) {
    auto* alloc = llvm::dyn_cast<llvm::AllocaInst>(array);
    arrayType = llvm::cast<llvm::ArrayType>(alloc->getAllocatedType());
  } else {
    throw CodeGenException(stmt->location(), "Not a valid array variable: " + name);
  }

  auto gep = _builder.CreateInBoundsGEP(arrayType, array,
                                        { _builder.getInt32(0), index },
                                        "element_ptr");

  stmt->value()->accept(*this);
  llvm::Value* valueToStore = _value;
  llvm::Type* targetType = arrayType->getElementType();
  if (_value->getType() != targetType) {
    if (targetType->isDoubleTy() && _value->getType()->isIntegerTy()) {
      valueToStore = _builder.CreateSIToFP(_value, targetType, "cast_double");
    } else if (targetType->isIntegerTy() && _value->getType()->isDoubleTy()) {
      valueToStore = _builder.CreateFPToSI(_value, targetType, "cast_int");
    } else {
      std::string errStr;
      llvm::raw_string_ostream errStream(errStr);
      errStream << "Unsupported type cast from " << *_value->getType()
                << " to " << *targetType;
      throw CodeGenException(stmt->location(), errStream.str());
    }
  }


  _builder.CreateStore(valueToStore, gep);
}

void CodeGenerator::visit(IfStmt* stmt) {
  stmt->condition()->accept(*this);
  auto condition = _value;
  // condition = _builder.CreateICmpNE(condition, _builder.getInt1(false), "ifcond");

  auto function = _builder.GetInsertBlock()->getParent();
  auto thenBlock = llvm::BasicBlock::Create(_context, "then", function);
  auto elseBlock = stmt->elseBranch() ? llvm::BasicBlock::Create(_context, "else", function) : nullptr;
  auto mergeBlock = llvm::BasicBlock::Create(_context, "ifcont", function);

  _builder.CreateCondBr(condition, thenBlock, elseBlock ? elseBlock : mergeBlock);
  
  // then
  _builder.SetInsertPoint(thenBlock);
  stmt->thenBranch()->accept(*this);
  bool thenTerminated = _builder.GetInsertBlock()->getTerminator();
  if(!thenTerminated)
    _builder.CreateBr(mergeBlock);
  
  // else
  if (stmt->elseBranch()) {
    _builder.SetInsertPoint(elseBlock);
    stmt->elseBranch()->accept(*this);
    if (!_builder.GetInsertBlock()->getTerminator())
      _builder.CreateBr(mergeBlock);
  }

  // merge
  _builder.SetInsertPoint(mergeBlock);
}

void CodeGenerator::visit(WhileStmt* stmt) {
  auto function = _builder.GetInsertBlock()->getParent();
  
  auto loopCond = llvm::BasicBlock::Create(_context, "loop.cond", function);
  auto loopBody = llvm::BasicBlock::Create(_context, "loop.body", function);
  auto loopEnd = llvm::BasicBlock::Create(_context, "loop.end", function);

  _builder.CreateBr(loopCond);

  // loop condition
  _builder.SetInsertPoint(loopCond);
  stmt->condition()->accept(*this);
  auto cond = _value;
  cond = _builder.CreateICmpNE(cond, _builder.getInt1(false), "while.cond");
  _builder.CreateCondBr(cond, loopBody, loopEnd);

  _breakBlock = loopEnd;

  // loop body
  _builder.SetInsertPoint(loopBody);
  stmt->body()->accept(*this);
  if (!_builder.GetInsertBlock()->getTerminator())
    _builder.CreateBr(loopCond);

  // loop end
  _builder.SetInsertPoint(loopEnd);
    _breakBlock = nullptr;
}

void CodeGenerator::visit(ForStmt* stmt) {
  auto function = _builder.GetInsertBlock()->getParent();

  // Evaluate start and end value
  stmt->start()->accept(*this);
  auto startVal = _value;
  stmt->end()->accept(*this);
  auto endVal = _value;

  // Allocate loop variable in entry block
  llvm::IRBuilder<> entryBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
  llvm::AllocaInst* loopVar = entryBuilder.CreateAlloca(_builder.getInt32Ty(), nullptr, stmt->loopVar());
  _builder.CreateStore(startVal, loopVar);
  _variables[stmt->loopVar()] = loopVar;

  // Create basic blocks
  llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(_context, "for.cond", function);
  llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(_context, "for.body", function);
  llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(_context, "for.end", function);

  // Initial jump to condition
  _builder.CreateBr(condBlock);

  // Condition block
  _builder.SetInsertPoint(condBlock);
  llvm::Value* curVal = _builder.CreateLoad(_builder.getInt32Ty(), loopVar, "loopvar");
  llvm::Value* cond;
  if (stmt->downto()) {
    cond = _builder.CreateICmpSGE(curVal, endVal, "cond");
  } else {
    cond = _builder.CreateICmpSLE(curVal, endVal, "cond");
  }
  _builder.CreateCondBr(cond, bodyBlock, endBlock);

  _breakBlock = endBlock;
  
  // Body block
  _builder.SetInsertPoint(bodyBlock);
  stmt->body()->accept(*this);
  
  _breakBlock = nullptr;

  // Increment or decrement
  llvm::Value* step = llvm::ConstantInt::get(_builder.getInt32Ty(), stmt->downto() ? -1 : 1);
  curVal = _builder.CreateLoad(_builder.getInt32Ty(), loopVar);
  llvm::Value* nextVal = _builder.CreateAdd(curVal, step, "nextval");
  _builder.CreateStore(nextVal, loopVar);
  _builder.CreateBr(condBlock);

  // End block
  _builder.SetInsertPoint(endBlock);
  _variables.erase(stmt->loopVar());
}

void CodeGenerator::visit(BlockStmt* stmt) {
  for (auto& stmt_ : stmt->statements())
    stmt_->accept(*this);
}

void CodeGenerator::visit(CallStmt* stmt) {
  emitCall(stmt->callee(), stmt->args(), stmt->location());
}

void CodeGenerator::visit(BreakStmt* stmt) { (void)stmt;
  if (_breakBlock) {
    _builder.CreateBr(_breakBlock);
    return;
  }
  throw CodeGenException(stmt->location(), "Break statement outside of loop");
}

void CodeGenerator::visit(ExitStmt* stmt) { (void)stmt;
  if (_returnBlock) {
    _builder.CreateBr(_returnBlock);
    return;
  }
  throw CodeGenException(stmt->location(), "Exit statement without return block");
}
