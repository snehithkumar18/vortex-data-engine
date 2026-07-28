#include "vde/record/schema_descriptor.h"

namespace vde {

void ComplexSchemaDescriptor::add_field(const std::string& name, FieldType type) {
    fields_.push_back({name, type, 0, {}});
}

void ComplexSchemaDescriptor::add_nested_struct(const std::string& name, std::vector<ComplexStructField> fields) {
    fields_.push_back({name, FieldType::Nested, 0, std::move(fields)});
}

std::string ComplexSchemaDescriptor::to_proto_string() const {
    std::string proto = "syntax = \"proto3\";\n\nmessage RecordData {\n";
    int tag = 1;
    for (const auto& f : fields_) {
        std::string t_str = "int32";
        if (f.type == FieldType::String) t_str = "string";
        else if (f.type == FieldType::Float64) t_str = "double";
        else if (f.type == FieldType::Int64) t_str = "int64";
        proto += "  " + t_str + " " + f.name + " = " + std::to_string(tag++) + ";\n";
    }
    proto += "}\n";
    return proto;
}

std::string ComplexSchemaDescriptor::to_json_schema() const {
    std::string json = "{\n  \"type\": \"object\",\n  \"properties\": {\n";
    for (size_t i = 0; i < fields_.size(); ++i) {
        const auto& f = fields_[i];
        json += "    \"" + f.name + "\": { \"type\": \"string\" }" + (i + 1 < fields_.size() ? "," : "") + "\n";
    }
    json += "  }\n}\n";
    return json;
}

std::string ComplexSchemaDescriptor::to_cpp_header() const {
    std::string cpp = "#pragma once\n#include <cstdint>\n#include <string>\n\nstruct GeneratedRecord {\n";
    for (const auto& f : fields_) {
        cpp += "  uint32_t " + f.name + ";\n";
    }
    cpp += "};\n";
    return cpp;
}

}
