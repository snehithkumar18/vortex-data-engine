#pragma once

#include "vde/record/schema.h"
#include <string>

namespace vde {

class SchemaCompiler {
public:
    SchemaCompiler() = default;

    Result<Schema> compile_from_json(std::string_view json_text);
    std::string serialize_to_json(const Schema& schema);
};

} // namespace vde
