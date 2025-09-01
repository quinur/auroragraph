#pragma once

#include <string_view>

#include "aurora/agql/ast.hpp"
#include "aurora/agql/errors.hpp"
#include "aurora/agql/token.hpp"

namespace aurora::agql {

Script parse_script(std::string_view input);

} // namespace aurora::agql

