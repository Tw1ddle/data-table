// A simple program to read, manipulate and print tables of data
// Copyright (C) 2021 Samuel Twidale
// License: GPLv3

#include <cassert>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "args/argparse.hpp"

#include "datasource/DataSource.h"
#include "datatable/DataTable.h"
#include "datatable/DataTablePrinter.h"

namespace
{

// Name of the command line argument for the data directory to look in
constexpr const char* dataFolderArgName = "data_dir";

// Helper function to get the path to the folder of CSV data to load
std::optional<std::filesystem::path> getCsvDataFolderPath(const argparse::ArgumentParser& parser)
{
    try {
        return parser.get<std::string>(::dataFolderArgName);
    } catch (const std::exception& err) {
        std::cerr << "Failed to retrieve input data folder path from command line with error: " << err.what() << std::endl;
    }
    return std::nullopt;
}

// Helper function to get the paths to the CSV files to load
std::vector<std::filesystem::path> getCsvFilePaths(const argparse::ArgumentParser& parser)
{
    const auto folderPath = getCsvDataFolderPath(parser);
    if (!folderPath) {
        return {};
    }
    std::vector<std::filesystem::path> filePaths;
    try {
        for (const auto& p : std::filesystem::directory_iterator(*folderPath)) {
            if (p.path().extension() == ".csv") {
                filePaths.emplace_back(p.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& err) {
        std::cerr << "Failed to enumerate CSV files in folder: " << folderPath->u8string() << " - with error: " << err.what() << std::endl;
    }
    return filePaths;
}

// Helper function to setup command line parser
argparse::ArgumentParser makeCommandLineArgsParser()
{
    argparse::ArgumentParser program("Molecular Data Table Demo");
    program.add_argument(::dataFolderArgName)
        .help("The directory to load the data table files from")
        .action([](const std::string& value) { return value; });
    return program;
}

int run(int argc, char* argv[])
{
    std::cout << "Running demo application" << std::endl;

    // Get CSV data folder from command line arg and load the CSV files

    argparse::ArgumentParser cli = ::makeCommandLineArgsParser();
    try {
        cli.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << "Failed to parse command line with error: " << err.what() << std::endl;
        std::cerr << cli;
        return -1;
    }

    std::cout << "Parsed command line arguments, will try to read CSV files" << std::endl;

    const auto& filePaths = getCsvFilePaths(cli);
    if (filePaths.empty()) {
        std::cerr << "Failed to find any CSV files" << std::endl;
        return -2;
    }

    std::cout << "Found " << filePaths.size() << " CSV files" << std::endl;

    // Load CSV files as molecular data tables and print them to console

    std::vector<std::pair<std::filesystem::path, datatable::DataTable>> tables;

    for (const auto& filePath : filePaths) {
        std::cout << "\n\n" << std::endl;

        std::cout << "Loading CSV from " << filePath.u8string() << std::endl;

        if (const auto table = datasource::readDataTableFromCsvFile(filePath)) {
            tables.emplace_back(std::make_pair(filePath, *table));
            datatable::printDataTable(*table);
        } else {
            std::cerr << "Failed to load table from CSV from " << filePath << std::endl;
            return -3;
        }

        std::cout << "\n\n" << std::endl;
    }


    // Perform some set operations on the data tables and print the results to console
    if (tables.size() < 2) {
        std::cerr << "Fewer than 2 tables loaded, so we can't try some set operations, exiting..." << std::endl;
        return -3;
    }

    const auto tablesUnion = std::make_pair("Set Union", [](const datatable::DataTable& first, const datatable::DataTable& second) -> datatable::DataTable {
        datatable::DataTable result;
        std::set_union(first.begin(), first.end(), second.begin(), second.end(), std::inserter(result, result.begin()), &datatable::compareRecords);
        return result;
    });

    const auto tablesDifference = std::make_pair("Set Difference", [](const datatable::DataTable& first, const datatable::DataTable& second) -> datatable::DataTable {
        datatable::DataTable result;
        std::set_difference(first.begin(), first.end(), second.begin(), second.end(), std::inserter(result, result.begin()), &datatable::compareRecords);
        return result;
    });

    const auto tablesSymmetricDifference = std::make_pair("Set Symmetric Difference", [](const datatable::DataTable& first, const datatable::DataTable& second) -> datatable::DataTable {
        datatable::DataTable result;
        std::set_symmetric_difference(first.begin(), first.end(), second.begin(), second.end(), std::inserter(result, result.begin()), &datatable::compareRecords);
        return result;
    });

    const auto tablesIntersection = std::make_pair("Set Intersection", [](const datatable::DataTable& first, const datatable::DataTable& second) -> datatable::DataTable {
        datatable::DataTable result;
        std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), std::inserter(result, result.begin()), &datatable::compareRecords);
        return result;
    });

    const std::vector<std::pair<std::string, std::function<datatable::DataTable(const datatable::DataTable&, const datatable::DataTable&)>>> setOps =
        { tablesUnion, tablesDifference, tablesSymmetricDifference, tablesIntersection };
    for (const auto& setOp : setOps) {
        std::cout << "\n\n" << "Will print " << setOp.first << " of tables from: " << tables[0].first << " and " << tables[1].first << "\n\n" << std::endl;
        const auto& tableOne = tables[0].second;
        const auto& tableTwo = tables[1].second;

        std::cout << "Table 1:" << std::endl;
        datatable::printDataTable(tableOne);
        std::cout << "\n" << std::endl;
        std::cout << "Table 2:" << std::endl;
        datatable::printDataTable(tableTwo);

        std::cout << "\n\n" << "Resulting table (" << setOp.first << ")" << std::endl;

        datatable::printDataTable(setOp.second(tableOne, tableTwo));
    }

    std::cout << "Successfully finished running demo application" << std::endl;
    return 0;
}

}

int main(int argc, char* argv[]) {
    try {
        return run(argc, argv);
    } catch (...) {
        assert(0 && "Unexpected exception");
        std::cerr << "Demo application finished with unexpected exception" << std::endl;
    }
    return -9999;
}