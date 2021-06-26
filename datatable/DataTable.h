#pragma once

#include <set>
#include <string>
#include <type_traits>
#include <variant>

namespace datatable
{

class Property {
public:
	Property(const std::string& key, double value) : key{ key }, value{ value } {}
	Property(const std::string& key, const std::string& value) : key{ key }, value{ value } {}

	std::string key;
	std::variant<double, std::string> value; // Properties are either numeric (using double only to keep things simple) or text (std::string)
};
// NOTE: keying on the property names, assumes there aren't duplicates with the same property name but different values.
inline bool compareProperties(const Property& first, const Property& second) {
	return first.key < second.key;
}
using PropertyComparator = std::integral_constant<decltype(&compareProperties), &compareProperties>;

class Record {
public:
	Record(const std::string& key, const std::string& name, const std::set<Property, PropertyComparator>& props) : key{ key }, name{ name }, props{ props } {}

	std::string key;
	std::string name;
	std::set<Property, PropertyComparator> props;
};
// NOTE: keying on the (molecule) names, assumes there aren't duplicates with the same name but different properties.
inline bool compareRecords(const Record& first, const Record& second) {
	return first.key < second.key;
}
using RecordComparator = std::integral_constant<decltype(&compareRecords), &compareRecords>;

using DataTable = std::set<Record, RecordComparator>;

}