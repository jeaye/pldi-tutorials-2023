#pragma once

#include <iostream>

enum scenario {
  scenario_default,
  scenario_specialized_inline,
  scenario_specialized_in_cpp,
};

template <scenario S> void foo() { std::cout << "foo<default>\n"; }

template <> void foo<scenario_specialized_in_cpp>();

template <> inline void foo<scenario_specialized_inline>() {
  std::cout << "foo<scenario_specialized_inline>\n";
}
