#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <stdexcept>

#include <json.hpp>

namespace aurora {

using Int = std::int64_t;
using Real = double;
using Bool = bool;
using Text = std::string;

using Value = std::variant<std::monostate, Int, Real, Bool, Text>;
using Properties = std::unordered_map<std::string, Value>;

inline std::string to_string(const Value& v) {
  return std::visit(
      [](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          return "null";
        } else if constexpr (std::is_same_v<T, Text>) {
          return arg;
        } else if constexpr (std::is_same_v<T, Bool>) {
          return arg ? "true" : "false";
        } else {
          return std::to_string(arg);
        }
      },
      v);
}

template <typename T>
inline const T* as(const Value& v) {
  return std::get_if<T>(&v);
}

inline void to_json(nlohmann::json& j, const Value& v) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
          j = nullptr;
        } else {
          j = arg;
        }
      },
      v);
}

inline void from_json(const nlohmann::json& j, Value& v) {
  if (j.is_null()) {
    v = std::monostate{};
  } else if (j.is_boolean()) {
    v = j.get<Bool>();
  } else if (j.is_number_integer()) {
    v = j.get<Int>();
  } else if (j.is_number_float()) {
    v = j.get<Real>();
  } else if (j.is_string()) {
    v = j.get<Text>();
  } else {
    throw std::runtime_error("Unsupported JSON value type");
  }
}

inline void to_json(nlohmann::json& j, const Properties& props) {
  j = nlohmann::json::object();
  for (const auto& [k, v] : props) {
    nlohmann::json val;
    to_json(val, v);
    j[k] = std::move(val);
  }
}

inline void from_json(const nlohmann::json& j, Properties& props) {
  props.clear();
  if (!j.is_object()) return;
  for (auto it = j.begin(); it != j.end(); ++it) {
    Value val;
    from_json(it.value(), val);
    props[it.key()] = std::move(val);
  }
}

} // namespace aurora
