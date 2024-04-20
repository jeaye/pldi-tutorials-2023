#include <clang/AST/Type.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Interpreter/CodeCompletion.h>
#include <clang/Interpreter/Interpreter.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/LineEditor/LineEditor.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ManagedStatic.h> // llvm_shutdown
#include <llvm/Support/Signals.h>
#include <llvm/Support/TargetSelect.h>

#include "prelude.hpp"

static void LLVMErrorHandler(void *UserData, char const *Message,
                             bool GenCrashDiag) {
  auto &Diags = *static_cast<clang::DiagnosticsEngine *>(UserData);

  Diags.Report(clang::diag::err_fe_error_backend) << Message;

  // Run the interrupt handlers to make sure any special cleanups get done, in
  // particular that we remove files registered with RemoveFileOnSignal.
  llvm::sys::RunInterruptHandlers();

  // We cannot recover from llvm errors.  When reporting a fatal error, exit
  // with status 70 to generate crash diagnostics.  For BSD systems this is
  // defined as an internal software error. Otherwise, exit with status 1.

  exit(GenCrashDiag ? 70 : 1);
}

llvm::ExitOnError ExitOnErr;

int main(int argc, char *argv[]) {
  ExitOnErr.setBanner("clang-repl: ");
  llvm::cl::ParseCommandLineOptions(argc, argv);

  llvm::llvm_shutdown_obj Y; // Call llvm_shutdown() on exit.

  // Initialize all targets (required for device offloading)
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  auto const pch_path("prelude.pch");
  std::vector<char const *> args{
      "-std=gnu++20",
      "-w",
      "-include-pch",
      pch_path,
  };

  clang::IncrementalCompilerBuilder CB;
  CB.SetCompilerArgs(args);
  auto CI(llvm::cantFail(CB.CreateCpp()));
  llvm::install_fatal_error_handler(LLVMErrorHandler,
                                    static_cast<void *>(&CI->getDiagnostics()));

  CI->LoadRequestedPlugins();

  auto Interp(llvm::cantFail(clang::Interpreter::create(std::move(CI))));

  llvm::LineEditor LE("clang-repl");
  std::string Input;
  while (std::optional<std::string> Line = LE.readLine()) {
    llvm::StringRef L = *Line;
    L = L.trim();
    if (L.ends_with("\\")) {
      Input += L.drop_back(1);
      LE.setPrompt("clang-repl...   ");
      continue;
    }

    Input += L;
    if (Input == R"(%quit)") {
      break;
    }
    if (Input == R"(%undo)") {
      if (auto Err = Interp->Undo()) {
        llvm::logAllUnhandledErrors(std::move(Err), llvm::errs(), "error: ");
      }
    } else if (Input.rfind("%lib ", 0) == 0) {
      if (auto Err = Interp->LoadDynamicLibrary(Input.data() + 5)) {
        llvm::logAllUnhandledErrors(std::move(Err), llvm::errs(), "error: ");
      }
    } else if (auto Err = Interp->ParseAndExecute(Input)) {
      llvm::logAllUnhandledErrors(std::move(Err), llvm::errs(), "error: ");
    }

    Input = "";
    LE.setPrompt("clang-repl> ");
  }
}
