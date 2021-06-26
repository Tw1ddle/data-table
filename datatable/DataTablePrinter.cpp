#include "../datatable/DataTablePrinter.h"

#include <iostream>
#include <string>
#include <variant>

#pragma warning(push)
#pragma warning(disable : 4267)
#include "../io/console/TextTable.h"
#pragma warning(pop)

#include "../datatable/DataTable.h"

namespace datatable
{

void printDataTable(const datatable::DataTable& table)
{
    if (table.empty()) {
        std::cout << "Table is empty" << std::endl;
        return;
    }

    TextTable t('-', '|', '+');

    // Build the table heading
    t.add("Key");
    t.add("Name");
    const auto& props = table.begin()->props;
    for (const auto& prop : props) {
        t.add(prop.key);
    }
    t.endOfRow();

    struct PropertyValueToString {
        std::string operator()(const std::string& x) const { return x; }
        std::string operator()(double x) const { return std::to_string(x); }
    };

    // Add the molecules and their properties
    for (const auto& record : table) {
        t.add(record.key);
        t.add(record.name);
        for (const auto& prop : record.props) {
            t.add(std::visit(PropertyValueToString(), prop.value));
        }
        t.endOfRow();
    }

    std::cout << t;
}

}