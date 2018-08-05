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

	bool create(notefile*, std::string);
	bool create(std::vector<note_value>*, std::string);
	bool destroy(std::shared_ptr<std::vector<notefile>>);

private:
	std::map<std::string,int> m_idx;
	template<typename T> struct dp_item {
		std::string name;
		T data;
	};
	template<typename T> struct dp_type_pool {
		std::vector<std::shared_ptr<dp_item<T>>> item;
	};

	dp_type_pool<std::vector<note_value>> m_rps {};
	dp_type_pool<std::vector<scd_t>> m_scds {};
	dp_type_pool<std::vector<frq_t>> m_frqs {};
	dp_type_pool<std::vector<ntstr_t>> m_ntstrs {};
	dp_type_pool<notefile> m_nfs {};
};

