#pragma once

#include <filesystem>
#include <optional>

#include "../datatable/DataTable.h"

namespace datasource
{

// Reads a data table from a CSV file, returns nullopt if the table is empty or it cannot be read
std::optional<datatable::DataTable> readDataTableFromCsvFile(const std::filesystem::path& csvPath);

}