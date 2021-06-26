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


    // Perform some set operations on the data tables and print to console

    // Union
    const auto unionTables = [](const datatable::DataTable& first, const datatable::DataTable& second) -> datatable::DataTable {
        datatable::DataTable result;
        std::set_union(first.begin(), first.end(), second.begin(), second.end(), std::inserter(result, result.begin()), &datatable::compareRecords);
        return result;
    };

    // Standard library implements the other set operations too

    if (tables.size() >= 2) {
        std::cout << "Will print combination of tables: " << tables[0].first << " and " << tables[1].first << std::endl;

        std::cout << "Table 1: " << tables[0].first << ":" << std::endl;
        datatable::printDataTable(tables[0].second);

        std::cout << "Table 2: " << tables[1].first << ":" << std::endl;
        datatable::printDataTable(tables[1].second);

        std::cout << "Combined table: " << std::endl;

        datatable::printDataTable(unionTables(tables[0].second, tables[1].second));
    }

    // To merge tables with different property columns, you would want to have a validation layer
    // that implements policies e.g. for "missing" columns, "bad" values e.g. NaN, range/regex-like checks etc
    // In this demo app, you might do something like that by extending the CSV importer code
    // In a real application, this is probably not something you'd do at level of individual importers/exporters
    // instead move the data through some grand central database

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