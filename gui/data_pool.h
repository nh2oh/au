#pragma once
#include "aulib\types\types_all.h"
#include "aulib\input\notefile.h"
#include <memory>
#include <vector>
#include <string>
#include <variant>

// Holds user-generated data
// Obviously this container only needs to manage aulib datatypes...
// it does not have to be _completely_ general...

class data_pool {
public:
	data_pool();

	bool create(notefile, std::string);
	bool create(std::vector<d_t>, std::string);
	bool destroy(std::string const);
	
	std::string print_nfs();
	std::vector<std::string> list_all() const;

private:
	template<typename T> struct dp_item {
		std::string name;  // A unique name identifying the dataset
		std::string description;  // Optional user-created description
		T data;
	};
	template<typename T> struct dp_type_pool {
		std::vector<dp_item<T>> items;
	};

	std::string name2uniquename(std::string) const;

	dp_type_pool<std::vector<d_t>> m_rps {};
	dp_type_pool<std::vector<scd_t>> m_scds {};
	dp_type_pool<std::vector<frq_t>> m_frqs {};
	dp_type_pool<std::vector<note_t>> m_ntstrs {};
	dp_type_pool<notefile> m_nfs {};
};

