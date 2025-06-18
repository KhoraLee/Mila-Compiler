#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <ast/visitor.h>
#include <ast/declaration.h>
#include <ast/expression.h>
#include <ast/statement.h>
#include <ast/types.h>

class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator();
    ~CodeGenerator() override = default;

    // Generate IR
    void generate(ProgramDecl* program) { program->accept(*this); }

    // Module and context management
    std::unique_ptr<llvm::Module> getModule() { return std::move(_module); }
    void setModule(std::unique_ptr<llvm::Module> module) { _module = std::move(module); }

    // Expression
    void visit(IntegerLiteral* expr) override;
    void visit(FloatLiteral* expr) override;
    void visit(StringLiteral* expr) override;
    void visit(VariableExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(UnaryExpr* expr) override;
    void visit(CallExpr* expr) override;
    void visit(ParenExpr* expr) override;
    void visit(ArrayAccess* expr) override;

    // Statement
    void visit(AssignStmt* stmt) override;
    void visit(ArrayAssignStmt* stmt) override;
    void visit(IfStmt* stmt) override;
    void visit(WhileStmt* stmt) override;
    void visit(ForStmt* stmt) override;
    void visit(BlockStmt* stmt) override;
    void visit(CallStmt* stmt) override;
    void visit(BreakStmt* stmt) override;
    void visit(ExitStmt* stmt) override;

    // Declaration
    void visit(ConstDecl* decl) override;
    void visit(VarDecl* decl) override;
    void visit(ArrayDecl* decl) override;
    void visit(FunctionDecl* decl) override;
    void visit(ProgramDecl* decl) override;

private:
    // LLVM context and module
    llvm::LLVMContext _context;
    llvm::IRBuilder<> _builder;
    std::unique_ptr<llvm::Module> _module;
  
    // Store last expreesion value
    llvm::Value* _value;
    llvm::BasicBlock* _currentReturnBlock;

    // Symbol table for variables and functions
    std::unordered_map<std::string, llvm::AllocaInst*> _variables;
    std::unordered_map<std::string, llvm::Constant*> _constants;
    std::unordered_map<std::string, llvm::GlobalVariable*> _globals;
    std::unordered_map<std::string, llvm::Function*> _functions;
    std::unordered_map<std::string, ArrayDecl*> _arrayDecls;

    llvm::Type* getLLVMType(const TokenType type);
    llvm::Constant* getLLVMDefaultValue(TokenType type);

    void initializeBuiltinFunctions();
    llvm::Value* emitCall(const std::string& callee, const std::vector<Expr>& args);
    llvm::Value* get_bin_expr_double(llvm::Value* left, llvm::Value* right, TokenType type);
    llvm::Value* get_bin_expr_int(llvm::Value* left, llvm::Value* right, TokenType type);
};
