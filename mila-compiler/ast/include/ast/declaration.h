#pragma once

#include <ast/type.h>
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
    Declaration(int line, int column) : ASTNode(line, column) {}
};

class NamedDecl : public Declaration {
public:
    NamedDecl(std::string name, TokenType type, int line, int column)
        : Declaration(line, column), _name(std::move(name)), _type(type) {}

    virtual bool isConst() const { return false; }
    virtual bool isArray() const { return false; }
    const std::string& name() const { return _name; }
    TokenType type() const { return _type; }

protected:
    std::string _name;
    TokenType _type;
};

using Named = std::shared_ptr<NamedDecl>;

class ConstDecl : public NamedDecl {
public:
    ConstDecl(std::string name, Expr value, int line, int column)
        : NamedDecl(std::move(name), TokenType::TOK_VOID, line, column),
          _value(std::move(value)) {}

    const Expr& value() const { return _value; }
    bool isConst() const override { return true; }

private:
    Expr _value;
};

class VarDecl : public NamedDecl {
public:
    using NamedDecl::NamedDecl;
};

class ArrayDecl : public NamedDecl {
public:
    ArrayDecl(std::string name, TokenType type, int start, int end, bool isConst, int line, int column)
        : NamedDecl(std::move(name), type, line, column), _start(start), _end(end), _isConst(isConst) {}
    int start() const { return _start; }
    int end() const { return _end; }
    bool isArray() const override { return true; }
    bool isConst() const override { return _isConst; }
private:
    int _start;
    int _end;
    bool _isConst;
};

// FunctionDecl includes Procedures (returnType == TokenType::TOK_VOID).
class FunctionDecl : public Declaration {
public:
    FunctionDecl(std::string name, ParamList params, TokenType returnType, 
                std::vector<Named> consts, std::vector<Named> vars, Block body, 
                int line, int column)
        : Declaration(line, column), _name(std::move(name)), _params(std::move(params)), 
          _returnType(returnType), _consts(std::move(consts)), _vars(std::move(vars)), _body(std::move(body)) {}
    
    const std::string& name() const { return _name; }
    const ParamList& params() const { return _params; }
    TokenType returnType() const { return _returnType; }
    const std::vector<Named>& consts() const { return _consts; }
    const std::vector<Named>& vars() const { return _vars; }
    const Block& body() const { return _body; }
private:
    std::string _name;
    ParamList _params;
    TokenType _returnType;
    std::vector<Named> _consts;
    std::vector<Named> _vars;
    Block _body;
};

using Function = std::shared_ptr<FunctionDecl>;

class ProgramDecl : public Declaration {
public:
    ProgramDecl(std::string name, std::vector<Named> consts, std::vector<Named> vars,
                std::vector<Function> functions, Block body, int line, int column)
        : Declaration(line, column), _name(std::move(name)), _consts(std::move(consts)),
         _vars(std::move(vars)), _funcs(std::move(functions)), _body(std::move(body)) {}
    const std::string& name() const { return _name; }
    const std::vector<Named>& consts() const { return _consts; }
    const std::vector<Named>& vars() const { return _vars; }
    const std::vector<Function>& functions() const { return _funcs; }
    const Block& body() const { return _body; }
private:
    std::string _name;
    std::vector<Named> _consts;
    std::vector<Named> _vars;
    std::vector<Function> _funcs;
    Block _body;
};
