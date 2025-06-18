#include <parser/parser.h>

#include <codegen/code_generator.h>

#include <iostream>
#include <fstream>

int main(int argc, char* args[]) {
  const char* fileName = args[1];
  std::ifstream file;
  file.open(fileName);

  Lexer lexer(file);
  Parser parser(lexer);
  CodeGenerator codegen;
  if (file.fail()) {
    std::cerr << "Couldn't open file\n" << fileName << std::endl;
    return 1;
  } else {
    try {
      auto program = parser.parse();
      program->accept(codegen);
      
      auto ir_module = codegen.getModule();
      ir_module->print(llvm::outs(), nullptr);
    } catch (MilaException e) {
      std::cerr << e.what() << std::endl;
    }
  }
}
