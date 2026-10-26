#ifndef COMPILER_STAGE
#define COMPILER_STAGE 5
#endif

#include "compiler/driver.hpp"
#include "compiler/lexer.hpp"
#if COMPILER_STAGE >= 2
#include "ast/top_level/program.hpp"
#include "compiler/parser.hpp"
#include "compiler/tools.hpp"
#endif
#if COMPILER_STAGE >= 3
#include "compiler/semantic_analysis.hpp"
#endif
#if COMPILER_STAGE >= 4
#include "compiler/ir_gen.hpp"
#include "tacky/top_level/program.hpp"
#endif
#if COMPILER_STAGE >= 5
#include "aast/top_level/program.hpp"
#include "compiler/codegen.hpp"
#endif
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace driver {
  /*
  This function calls the GCC driver to remove comments and trims whitespace in the C file provided.
  This makes it easier for the compiler to parse and lex the C file.
  */
  void preprocess(const std::string& filename) {
    if (!fork()) {
      execlp("gcc", "gcc", "-E", "-P", filename.c_str(), "-o", PREPROCESSED_FILE.data(), (char*)nullptr);
      _exit(1);
    } else {
      wait(nullptr);
    }
  }

  // This function removes the intermediate files
  void cleanup(bool remove_object_files, std::vector<char*>& object_filenames) {
    if (!fork()) {
      std::vector<char*> cmd_list{const_cast<char*>("rm"), const_cast<char*>(PREPROCESSED_FILE.data()), const_cast<char*>(ASSEMBLY_FILE.data())};
      if (remove_object_files) {
        for (char* filename : object_filenames) cmd_list.push_back(filename);
      }
      cmd_list.push_back(nullptr);
      execvp("rm", cmd_list.data());
      _exit(1);
    } else {
      wait(nullptr);
    }
  }

  // This function converts Assembly File To Binary Executable
  void convert_to_execute(const std::vector<char*>& cmd_list) {
    if (!fork()) {
      execvp("gcc", cmd_list.data());
      _exit(1);
    } else {
      wait(nullptr);
    }
  }

  // This function converts Assembly File TO Object File
  void convert_to_object(const std::string& filename) {
    if (!fork()) {
      execlp("gcc", "gcc", "-c", ASSEMBLY_FILE.data(), "-o", filename.c_str(), (char*)nullptr);
      _exit(1);
    } else {
      wait(nullptr);
    }
  }
} // namespace driver

int main(int argc, char* argv[]) {
  if (argc < 2) { throw std::runtime_error("Missing Filename."); }
  try {
    driver::Stage stop = driver::Stage::Full;
    [[maybe_unused]] bool is_binary = true;
    int i = 1;
    for (; i < argc; ++i) {
      std::string arg(argv[i]);
      if (arg == "-c") is_binary = false;
      else if (arg == "--lex") stop = driver::Stage::Lex;
      else if (arg == "--parse") stop = driver::Stage::Parse;
      else if (arg == "--validate") stop = driver::Stage::Validate;
      else if (arg == "--tacky") stop = driver::Stage::Tacky;
      else if (arg == "--codegen") stop = driver::Stage::Codegen;
      else break;
    }

    std::list<std::string> tokens;
#if COMPILER_STAGE >= 2
    std::unique_ptr<ast::Program> program;
#endif
#if COMPILER_STAGE >= 4
    std::unique_ptr<tacky::Program> tacky_program;
#endif
#if COMPILER_STAGE >= 5
    std::unique_ptr<aast::Program> assembly_program;
    std::ofstream ostr;
    std::vector<std::string> object_filenames;
    std::string output_filename;
#endif
    for (; i < argc; ++i) {
      std::string filename(argv[i]);
#if COMPILER_STAGE >= 2
      tools::frontend_symbols.clear(); // clear the symbols map to avoid conflicts between files
#endif
      driver::preprocess(filename);
      tokens = lexer::lex(std::string(driver::PREPROCESSED_FILE));
      if (stop == driver::Stage::Lex) continue;

#if COMPILER_STAGE >= 2
      program.reset(parser::parse(tokens));
      if (stop == driver::Stage::Parse) continue;
#endif

#if COMPILER_STAGE >= 3
      semantic_analysis::analyze_program(program.get());
      if (stop == driver::Stage::Validate) continue;
#endif

#if COMPILER_STAGE >= 4
      tacky_program.reset(ir_gen::generate_tacky(program.get()));
      if (stop == driver::Stage::Tacky) continue;
#endif

#if COMPILER_STAGE >= 5
      assembly_program.reset(codegen::generate_top_level(tacky_program.get()));
      if (stop == driver::Stage::Codegen) continue;
#endif

#if COMPILER_STAGE >= 6
      ostr.open(driver::ASSEMBLY_FILE.data());
      if (!ostr) { throw std::runtime_error("Failed To Open Assembly File"); }
      ostr << *assembly_program; // Writing Assembly To File using Output Stream Extraction Operator Overloading
      ostr.close();

      filename.erase(filename.size() - 2);
      if (output_filename.empty()) output_filename = filename;
      filename += ".o";
      driver::convert_to_object(filename);
      object_filenames.push_back(filename);
#endif
    }
#if COMPILER_STAGE >= 6
    if (stop != driver::Stage::Full) { return EXIT_SUCCESS; }
    std::vector<char*> object_filenames_as_args;
    for (const std::string& filename : object_filenames) object_filenames_as_args.push_back(const_cast<char*>(filename.c_str()));

    if (is_binary) {
      std::vector<char*> cmd_list;
      cmd_list.push_back(const_cast<char*>("gcc"));
      cmd_list.insert(cmd_list.end(), object_filenames_as_args.begin(), object_filenames_as_args.end());
      cmd_list.push_back(const_cast<char*>("-o"));
      cmd_list.push_back(const_cast<char*>(output_filename.c_str()));
      cmd_list.push_back(nullptr);
      driver::convert_to_execute(cmd_list);
    }
#ifndef DEBUG
    driver::cleanup(is_binary, object_filenames_as_args);
#endif
#endif
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
