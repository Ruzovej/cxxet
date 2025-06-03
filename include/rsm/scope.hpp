#pragma once

namespace rsm {

enum class scope_t : char {
  global = 'g',
  process = 'p',
  thread = 't',
};

}
