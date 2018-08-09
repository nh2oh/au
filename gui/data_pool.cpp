#include "data_pool.h"
#include "aulib\input\notefile.h"
#include <vector>
#include <memory>
#include <string>


data_pool::data_pool() {
	//...
}

bool data_pool::create(notefile nf_in, std::string name) {
	dp_item<notefile> item {name, std::string {""}, nf_in};

	m_nfs.items.push_back(item);
	return true;
}

bool data_pool::create(std::vector<note_value> rp_in, std::string name) {
	dp_item<std::vector<note_value>> item {name, std::string {""}, rp_in};

	m_rps.items.push_back(item);
	return true;
}

bool data_pool::destroy(std::shared_ptr<std::vector<notefile>> spnf) {
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

std::vector<std::string> data_pool::list_all() {
	std::vector<std::string> vs {};

	std::string nf_suffix = "NF:  ";
	for (auto e : m_nfs.items) {
		vs.push_back(nf_suffix + e.name);
	}

	std::string rp_suffix = "RP:  ";
	for (auto e : m_rps.items) {
		vs.push_back(rp_suffix + e.name);
	}

	return vs;
}

