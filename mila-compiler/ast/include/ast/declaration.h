#pragma once

#include <ast/types.h>
#include <ast/node.h>
#include <ast/visitor.h>
#include <ast/statement.h>
#include <ast/expression.h>
#include <string>
#include <memory>
#include <vector>

class Declaration : public ASTNode {
public:
    virtual ~Declaration() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitDeclaration(this);
    }
    Declaration(SourceLocation loc) : ASTNode(std::move(loc)) {}
};

class NamedDecl : public Declaration {
public:
    NamedDecl(std::string name, TokenType type, SourceLocation loc)
        : Declaration(std::move(loc)), _name(std::move(name)), _type(type) {}

    virtual bool isConst() const { return false; }
    virtual bool isArray() const { return false; }
    const std::string& name() const { return _name; }
    TokenType type() const { return _type; }

protected:
    std::string _name;
    TokenType _type;
};

using Named_D = std::shared_ptr<NamedDecl>;

class ConstDecl : public NamedDecl {
public:
    ConstDecl(std::string name, Expr value, SourceLocation loc)
        : NamedDecl(std::move(name), TokenType::TOK_VOID, std::move(loc)),
          _value(std::move(value)) {}

    const Expr& value() const { return _value; }
    bool isConst() const override { return true; }

private:
    Expr _value;
};

using Const_D = std::shared_ptr<ConstDecl>;

class VarDecl : public NamedDecl {
public:
    using NamedDecl::NamedDecl;
};

using Var_D = std::shared_ptr<VarDecl>;

class ArrayDecl : public NamedDecl {
public:
    ArrayDecl(std::string name, TokenType type, int start, int end, bool isConst, SourceLocation loc)
        : NamedDecl(std::move(name), type, std::move(loc)), _start(start), _end(end), _isConst(isConst) {}
    int start() const { return _start; }
    int end() const { return _end; }
    bool isArray() const override { return true; }
    bool isConst() const override { return _isConst; }
private:
    int _start;
    int _end;
    bool _isConst;
};

using Array_D = std::shared_ptr<ArrayDecl>;

// FunctionDecl includes Procedures (returnType == TokenType::TOK_VOID).
class FunctionDecl : public Declaration {
public:
    FunctionDecl(std::string name, ParamList params, TokenType returnType, 
                std::vector<Named_D> consts, std::vector<Named_D> vars, Block_S body, 
                SourceLocation loc)
        : Declaration(std::move(loc)), _name(std::move(name)), _params(std::move(params)), 
          _returnType(returnType), _consts(std::move(consts)), _vars(std::move(vars)), _body(std::move(body)) {}
    
    const std::string& name() const { return _name; }
    const ParamList& params() const { return _params; }
    TokenType returnType() const { return _returnType; }
    const std::vector<Named_D>& consts() const { return _consts; }
    const std::vector<Named_D>& vars() const { return _vars; }
    const Block_S& body() const { return _body; }
private:
    std::string _name;
    ParamList _params;
    TokenType _returnType;
    std::vector<Named_D> _consts;
    std::vector<Named_D> _vars;
    Block_S _body;
};

using Function_D = std::shared_ptr<FunctionDecl>;

class ProgramDecl : public Declaration {
public:
    ProgramDecl(std::string name, std::vector<Named_D> consts, std::vector<Named_D> vars,
                std::vector<Function_D> functions, Block_S body, SourceLocation loc)
        : Declaration(std::move(loc)), _name(std::move(name)), _consts(std::move(consts)),
         _vars(std::move(vars)), _funcs(std::move(functions)), _body(std::move(body)) {}
    const std::string& name() const { return _name; }
    const std::vector<Named_D>& consts() const { return _consts; }
    const std::vector<Named_D>& vars() const { return _vars; }
    const std::vector<Function_D>& functions() const { return _funcs; }
    const Block_S& body() const { return _body; }
private:
    std::string _name;
    std::vector<Named_D> _consts;
    std::vector<Named_D> _vars;
    std::vector<Function_D> _funcs;
    Block_S _body;
};

using Program_D = std::shared_ptr<ProgramDecl>;
