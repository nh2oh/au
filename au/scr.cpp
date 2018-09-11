#include "scr.h"
#include <vector>
#include <numeric>





double neumaierSum(std::vector<double> input) {
	double sum {0.0};
	double c {0.0};
	for (int i=0; i<input.size(); ++i) {
		double t = sum + input[i];
		if (std::abs(sum) >= std::abs(input[i])) {
			c += (sum-t) + input[i];
		} else {
			c += (input[i]-t) + sum;
		}
		sum = t;
	}
	return sum + c;
}




/*


TEST(ts_t_tests, ThreeFourIterativeNbarCalculations) {
ts_t ts {3_bt,d::q,false};

bar_t cum_nbars {0};
for (int i=1; i<100; ++i) {  // Note: starting at 1
cum_nbars += nbar(ts, d_t{d::q});

if (cum_nbars.isexact()) {
cum_nbars = cum_nbars.full();
}


if (i%3==0) {
bool isex = cum_nbars.isexact(); EXPECT_TRUE(isex);
double frem = cum_nbars.fremain(); EXPECT_EQ(frem,1.0);
bar_t nfull = cum_nbars.full(); EXPECT_EQ(nfull,(i/3)*1_br);
bar_t nnext = cum_nbars.next(); EXPECT_EQ(nnext,(i/3)*1_br+1_br);
EXPECT_EQ(cum_nbars,(i/3)*(1_br));

if (!(nfull == (i/3)*1_br)) {
auto a = std::round(cum_nbars.to_double());
auto b = cum_nbars.to_double();
auto e = std::numeric_limits<double>::epsilon();
auto m = std::numeric_limits<double>::min();
auto d = std::abs(a-b);
auto s = std::abs(a+b);
int aa = 1+1;
}
}
}

}















*/


