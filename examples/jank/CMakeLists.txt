add_executable(jank jank.cpp prelude.cpp)
set_property(TARGET jank PROPERTY CXX_STANDARD 17)
target_include_directories(jank PRIVATE ${CMAKE_SOURCE_DIR}/examples)
target_include_directories(jank PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(jank PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(jank PRIVATE clang-cpp clangInterpreter)
set_target_properties(jank PROPERTIES ENABLE_EXPORTS 1)

add_custom_command(
  TARGET jank
  COMMAND ${Clang_DIR}/../../../bin/clang++ -std=gnu++20 -w -Xclang -fincremental-extensions -Xclang -emit-pch -x c++-header -o ${CMAKE_BINARY_DIR}/prelude.pch -c ${CMAKE_SOURCE_DIR}/examples/jank/prelude.hpp
)
