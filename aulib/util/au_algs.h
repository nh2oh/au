#pragma once
#include <vector>
#include <algorithm>
//-----------------------------------------------------------------------------
// ismember()

// Scalar - vector
template<typename T> bool ismember(T const&, std::vector<T> const&);
template<typename T> std::vector<bool> ismember(std::vector<T> const&, std::vector<T> const&);

template<typename T>
bool ismember(T const& a, std::vector<T> const& vb) {
	for (auto const& cb : vb) {
		if (a == cb) {
			return true;
		}
	}
	return false;
}

// Returns a vector the size of va where each element is true if the
// corresponding element of va is a member of vb
template<typename T>
std::vector<bool> ismember(std::vector<T> const& va, std::vector<T> const& vb) {
	auto va_ismember = std::vector<bool>(va.size(), false);
	size_t i = 0;
	for (auto const& cva : va) {
		va_ismember[i] = ismember<T>(cva, vb);
		++i;
	}
	return va_ismember;
}

// Returns a vector the size of va where each element is true if it is
// equal to the value b (equivalent to the above where vb is a 1-element
// vector).  
template<typename T>
std::vector<bool> ismember(std::vector<T> const& va, T const& b) {
	auto va_ismember = std::vector<bool>(va.size(), false);
	size_t i = 0;
	for (auto const& cva : va) {
		va_ismember[i] = (b == cva);
		++i;
	}
	return va_ismember;
}

//-----------------------------------------------------------------------------
// unique()-like functions

// unique():  Returns the unique elements in v
template<typename T> std::vector<T> unique(std::vector<T>);

template<typename T>
std::vector<T> unique(std::vector<T> v) {
	std::sort(v.begin(),v.end());  // std::includes() requires v be sorted

	std::vector<T> uq_v {};
	for (int i=0; i<v.size(); ++i) {
		if (!std::includes(uq_v.begin(),uq_v.end(),v.begin()+i,v.begin()+i+1)) {
			uq_v.push_back(v[i]);
		}
	}
	return uq_v;
}

// unique_n():  Returns the unique elements in v and their counts
template<typename T> struct unique_n_result {
	std::vector<T> values;
	std::vector<int> counts;
};

template<typename T> unique_n_result<T>unique_n(std::vector<T>);

template<typename T>
unique_n_result<T> unique_n(std::vector<T> v) {
	std::sort(v.begin(),v.end());  // std::includes() requires v be sorted

	unique_n_result<T> uq_v {};
	for (int i=0; i<v.size(); ++i) {
		auto uq_it = std::find(uq_v.values.begin(),uq_v.values.end(),v[i]);
		if (uq_it == uq_v.values.end()) {
			uq_v.values.push_back(v[i]);
			uq_v.counts.push_back(std::count(v.begin(),v.end(),v[i]));
		}
	}
	
	return uq_v;
}

// n_unique():  Counts the # of unique elements in v
template<typename T> size_t n_unique(std::vector<T>);

template<typename T>
size_t n_unique(std::vector<T> v) {
	double n = v.size(); // number of unique elements
	double cc = 0; // "current count" => # of duplicate members on each
				   // iteration.  

	for (auto const& e : v) {
		cc = static_cast<double>(std::count(v.begin(), v.end(), e)); // cc ~ 
		if (cc > 1) { n = n-((cc-1)/cc); }
	}
	return static_cast<size_t>(std::abs(std::round(n)));
}

//-----------------------------------------------------------------------------
// isidenticalset

template<typename T> bool isidenticalset(std::vector<T>, std::vector<T>);

template<typename T>
bool isidenticalset(std::vector<T> va, std::vector<T> vb) {
	return isall(ismember(va,vb));
}

//-----------------------------------------------------------------------------
// issubset, iseithersubset

// Returns true if va is a subset of vb (all members of va exist in vb), false
// otherwise.
template<typename T> bool issubset(std::vector<T> va, std::vector<T> vb);

template<typename T>
bool issubset(std::vector<T> va, std::vector<T> vb) {
	for (auto const& ca : va) {
		for (auto ib = vb.begin(); ib != vb.end(); ++ib) {
			if (ca == *ib) { break;	}
			// if the present element of vb, *ib, is the last...
			if ((ib+1) == vb.end()) { return false; }
		}
	}
	return true;
}


// Is _either_ input a subset of the other?
// Returns true if va is a subset of vb _or_ if vb is a subset of va
template<typename T> bool iseithersubset(std::vector<T> va, std::vector<T> vb);

template<typename T>
bool iseithersubset(std::vector<T> va, std::vector<T> vb) {
	auto b_found_in_a = std::vector<size_t>(vb.size(),0);
	size_t n_a_in_b = 0; size_t n_b_in_a = 0;
	for (auto const& ca : va) {
		size_t idx_b = 0;
		bool f_found_ca = false; // "flag found [element] ca" [in b]
		for (auto const& cb : vb) {
			if ((ca == cb) && (b_found_in_a[idx_b]==0)) {
				// ca is in b _and_ idx_b was _not_ previously known to match
				// a member of a
				f_found_ca = true;
				b_found_in_a[idx_b] = 1;
				++n_b_in_a;
				if (n_b_in_a == vb.size()) {
					return true;  // b is a subset of a
				}
			} else if ((ca == cb) && (b_found_in_a[idx_b]==1)) {
				// ca is in b _but_ idx_b was previously known to match
				// a member of a.  ca must be a duplicate of a previous
				// element in a matching idx_b.  There is no need to search
				// the rest of b, since b was exhaustively searched the
				// first time the match was encountered.  
				f_found_ca = true;
				break;
			}
			++idx_b;
		} // to next cb in b
		// Done checking b for ca
		
		if (f_found_ca) {
			++n_a_in_b;
		}
	} // to next ca in a

	if (n_a_in_b == va.size()) {
		return true; // a is a subset of b
	}

	return false;
}

//-----------------------------------------------------------------------------
// isany() and isall()

bool isany(std::vector<bool>);
bool isall(std::vector<bool>);

//-----------------------------------------------------------------------------
// esubs()

// bool2idx:  Converts a std::vector<bool> to a std::vector<int> of the idx of
// the true elements of the input
std::vector<int> bool2idx(std::vector<bool>);

// esubs():  Return the elements of v_in corresponding to the subscripts idx
template<typename T> std::vector<T> esubs(std::vector<T>, 
	std::vector<size_t>);

// idx are numeric size_t indices
template<typename T>
std::vector<T> esubs(std::vector<T> v_in, std::vector<size_t> idx) {
	auto v_out = std::vector<T>(); v_out.reserve(v_in.size());

	for (auto const& cidx : idx) {
		v_out.push_back(v_in[cidx]);
	}

	return v_out;
};


