#include "data_pool.h"
#include "aulib\input\notefile.h"
#include "aulib\util\au_algs.h"
#include <vector>
#include <memory>
#include <string>


data_pool::data_pool() {
	//...
}

bool data_pool::create(notefile nf_in, std::string name) {
	dp_item<notefile> new_item {name, std::string {""}, nf_in};
	new_item.name = name2uniquename(new_item.name);

	m_nfs.items.push_back(new_item);
	return true;
}

bool data_pool::create(std::vector<nv_t> rp_in, std::string name) {
	dp_item<std::vector<nv_t>> new_item {name, std::string {""}, rp_in};
	new_item.name = name2uniquename(new_item.name);

	m_rps.items.push_back(new_item);
	return true;
}

bool data_pool::destroy(std::string const name) {
	for (auto it=m_nfs.items.begin(); it!=m_nfs.items.end(); ++it) {
		if (name == (*it).name) {
			m_nfs.items.erase(it);
			return true;
		}
	}

	for (auto it=m_rps.items.begin(); it!=m_rps.items.end(); ++it) {
		if (name == (*it).name) {
			m_rps.items.erase(it);
			return true;
		}
	}

	return false;
}


std::string data_pool::print_nfs() {
	std::string s {};
	for (auto e : m_nfs.items) {
		s += e.name;
		s += "\n";
	}

	return s;
}

std::vector<std::string> data_pool::list_all() const {
	std::vector<std::string> vs {};

	for (auto e : m_nfs.items) {
		vs.push_back(e.name);
	}

	for (auto e : m_rps.items) {
		vs.push_back(e.name);
	}

	return vs;
}

std::string data_pool::name2uniquename(std::string name) const {
	auto all_names = list_all();
	std::string suffix {}; int i=1;
	while (ismember((name+suffix), all_names)) {
		suffix = "_" + std::to_string(i);
		++i;
	}
	return (name += suffix);
}




