# Data Tables

Toy application for representing collections of molecules and associated properties:

 * Implements a simple data table class that can represent collections of molecules with associated properties.
 * Supports common set operations on the tables, even those that have different property columns.
 * This is a simple demo application to show these two features.
 
# Usage

 * Build and run the demo application in the console, providing a path to a folder of CSV files.

# Discussion

 * To take this further, would probably have adapters for various databases and use those for import/export. Used CSV parser to keep this real simple.
 * Assumed that the tables are always going to be small enough to hold in memory (i.e. not billions of entries at once).
 * There's no sanity checking/validation on the data e.g. handling missing columns when importing CSV files.

# Notes

 * Built for Windows under MSVC 2019 Enterprise version 16.10.2.
 * Includes demo (main.cpp).
 * Uses some header-only libraries for CSV parsing, CLI argument parsing, printing pretty tables to console.