#pragma once

namespace cxxet {

enum class scope_t : char {
  global = 'g',
  process = 'p',
  thread = 't',
};

}
