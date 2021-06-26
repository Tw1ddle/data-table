#include "DataSource.h"

#include <filesystem>
#include <iostream>
#include <optional>

#pragma warning(push)
#pragma warning(disable:4244 4267 4458)
#include "../io/csv/csv.h"
#pragma warning(pop)

#include "../datatable/DataTable.h"

namespace datasource
{

std::optional<datatable::DataTable> readDataTableFromCsvFile(const std::filesystem::path& csvPath)
{
	try {
		io::CSVReader<3> in(csvPath.u8string().c_str());
		in.read_header(io::ignore_extra_column | io::ignore_missing_column | io::ignore_no_column, "Molecule", "Solubility", "Molecular Weight");

		std::string moleculeName;
		double solubility;
		double molecularWeight;

		datatable::DataTable table;
		while (in.read_row(moleculeName, solubility, molecularWeight)) {
			table.insert(datatable::Record(moleculeName, moleculeName,
				{ datatable::Property("Solubility", solubility), datatable::Property("Molecular Weight", molecularWeight) }));
		}

		if (table.empty()) {
			return std::nullopt;
		}
		return std::make_optional(table);
	} catch (const std::exception& err) {
		std::cerr << "Failed to read CSV data table with error: " << err.what() << std::endl;
	}

	return std::nullopt;
}

}