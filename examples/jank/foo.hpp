#pragma once

template <typename T>
T foo(T t)
{ return t; }

/* If this isn't `extern template`, it JIT links correctly. */
extern template char foo(char c);
