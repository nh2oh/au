#pragma once

namespace auexp {


class sc_t_base {

};

class sc_diatonic_t : public sc_t_base {
public:
	static constexpr int get_N();
	static const int N;
};

template<typename SC>
struct scd_t {
	int value_;
};

template<typename SC>
scd_t<SC> reduce(scd_t<SC>) {
	//...
};

};  // namespace auexp
