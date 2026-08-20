#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace driver {
  enum class Stage { Lex, Parse, Validate, Tacky, Codegen, Full };
  constexpr std::string_view PREPROCESSED_FILE = "preprocessed.i";
  constexpr std::string_view ASSEMBLY_FILE = "assembly.s";
  void preprocess(const std::string& filename);
  void cleanup(bool remove_object_files, std::vector<char*>& object_filenames);
  void convert_to_execute(const std::vector<char*>& cmd_list);
  void convert_to_object(const std::string& filename);
} // namespace driver
