#pragma once

#include "vde/common/types.h"
#include "vde/record/schema.h"
#include "vde/record/field_value.h"
#include <string>
#include <vector>

namespace vde {

struct ComplexStructField {
    std::string name;
    FieldType type;
    uint32_t length;
    std::vector<ComplexStructField> children;
};

class ComplexSchemaDescriptor {
public:
    ComplexSchemaDescriptor() = default;

    void add_field(const std::string& name, FieldType type);
    void add_nested_struct(const std::string& name, std::vector<ComplexStructField> fields);

    size_t field_count() const { return fields_.size(); }
    const ComplexStructField& get_field(size_t index) const { return fields_[index]; }

    std::string to_proto_string() const;
    std::string to_json_schema() const;
    std::string to_cpp_header() const;

private:
    std::vector<ComplexStructField> fields_;
};

}
