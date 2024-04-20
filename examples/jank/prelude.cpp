#include "prelude.hpp"

template <> void foo<scenario_specialized_in_cpp>() {
  std::cout << "foo<scenario_specialized_in_cpp>\n";
}
