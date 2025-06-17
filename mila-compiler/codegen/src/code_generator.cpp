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
            return _builder.getFloatTy();
        case TOK_STRING:
            return _builder.getPtrTy();
        default:
            return _builder.getVoidTy();
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
