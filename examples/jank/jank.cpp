#include <clang/Frontend/CompilerInstance.h>
#include <clang/Interpreter/Interpreter.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>

int main(int argc, char *argv[]) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  std::vector<char const *> args{
      "-std=gnu++20",
      "-w",
  };

  clang::IncrementalCompilerBuilder CB;
  CB.SetCompilerArgs(args);
  auto CI(llvm::cantFail(CB.CreateCpp()));

  CI->LoadRequestedPlugins();

  auto Interp(llvm::cantFail(clang::Interpreter::create(std::move(CI))));

  Interp->ParseAndExecute("#include \"../examples/jank/foo.hpp\"");
  /* Works. */
  Interp->ParseAndExecute("foo(1);");
  /* Fails. */
  Interp->ParseAndExecute("foo('a');");
}
