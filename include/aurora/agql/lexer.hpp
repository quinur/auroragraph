#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "aurora/agql/errors.hpp"
#include "aurora/agql/token.hpp"

namespace aurora::agql {

std::vector<Token> lex(std::string_view input);

} // namespace aurora::agql

