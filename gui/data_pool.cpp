#include "data_pool.h"
#include <vector>
#include <memory>
#include <string>


data_pool::data_pool() {
	//...
}

bool data_pool::create(notefile *pnf, std::string name) {
	auto item = new dp_item<notefile>;
	(*item).data = *pnf;
	(*item).name = name;

	m_nfs.item.push_back(std::make_shared<dp_item<notefile>>(*item));
	return true;
}

bool data_pool::create(std::vector<note_value> *prp, std::string name) {
	auto item = new dp_item<std::vector<note_value>>;
	(*item).data = *prp;
	(*item).name = name;

	m_rps.item.push_back(std::make_shared<dp_item<std::vector<note_value>>>(*item));
	return true;
}

bool data_pool::destroy(std::shared_ptr<std::vector<notefile>> spnf) {
	return false;
}
