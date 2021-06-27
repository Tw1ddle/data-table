# Data Tables

Toy application for representing collections of molecules and associated properties:

 * Implements a simple data table type that can represent collections of molecules with associated properties.
 * The data table type is a set type that supports common set operations.
 * This is a simple demo application to show these two features.
 
# Usage

 * Build and run the demo application in the console, providing a path to a folder of CSV files. It will print the union, difference, symmetric difference and intersection of the tables.
 
 There's a prebuilt 64-bit Windows release binary at the root of the repository.
 
```./MolecularDataTables.exe ./data```

The output should look like this:

![GIF of it working](screenshots/usage_and_output.gif)

# Discussion

 * Note that the names of the molecules are used as keys in this simple implementation. I could permit duplicates in various senses by using instance UUIDs for keys, or perhaps comparing all the fields instead of just the molecule/property names.
 
 # Further Work
 
To take this further, would probably have adapters for various databases and use those for import/export. Used CSV parser to keep this real simple.

To merge tables with different property columns, you would want to have a validation layer that implements policies e.g. for how to fill in "missing" data, handling "bad" values e.g. NaN, range checks, regex matching etc. In a simple application like this demo app, you might manage that by extending the importer code (DataSource.h/cpp). For a serious application, this is probably not something you'd do at level of individual importers/exporters. Instead, move the data through a grand central database that performs validation.

There might also be a layer that does type inference to decide whether a particular property should be treated as numeric
or as a string. A property can have numeric values but actually be a string in the most general case i.e. "how long is a piece of string?"

I have assumed that the tables are always going to be small enough to hold in memory (i.e. not billions of entries at once), and have
chosen data structures for simplicity rather than performance in mind. If the focus were on performance, would do things very differently to minimize allocations etc.

# Notes

 * Built for Windows under MSVC 2019 Enterprise version 16.10.2.
 * Uses some open source header-only libraries for CSV parsing, CLI argument parsing, printing pretty tables to console.