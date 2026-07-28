#include "vde/pipeline/pipeline.h"
#include "vde/query/sql_lexer.h"
#include "vde/query/sql_parser.h"
#include "vde/query/query_planner.h"
#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: vdx_query_cli <file.vdx> \"<SQL Query>\"" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) return 1;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<vde::byte_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    vde::Pipeline pipeline;
    vde::Status st = pipeline.process(vde::Span<const vde::byte_t>(buffer.data(), buffer.size()));
    if (st != vde::Status::Ok) {
        std::cerr << "Error parsing VDX container" << std::endl;
        return 1;
    }

    vde::SqlLexer lexer(argv[2]);
    auto tokens = lexer.tokenize();
    vde::SqlParser parser(std::move(tokens));
    auto select_res = parser.parse_select();

    if (!select_res.has_value()) {
        std::cerr << "SQL Parse Error" << std::endl;
        return 1;
    }

    vde::QueryPlanner planner;
    auto plan = planner.create_plan(*select_res.value, &pipeline.records());
    if (plan) {
        plan->open();
        size_t count = 0;
        while (true) {
            auto rec_res = plan->next();
            if (!rec_res.has_value()) break;
            count++;
            std::cout << "Row " << count << ": ID=" << rec_res.value.id << std::endl;
        }
        plan->close();
    }

    return 0;
}
