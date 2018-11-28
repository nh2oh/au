#include "gtest/gtest.h"
#include "..\aulib\types\cent_oct_t.h"
#include "..\aulib\types\frq_t.h"


//
// TODO:  Tests comparing oct_t, cent_t
//

// Default ctor
TEST(oct_t_tests, DefaultCtor) {
	oct_t o {};
	EXPECT_EQ(o.to_int(),0);
	EXPECT_EQ(o,oct_t{0});
}

// Ctor frq ratios
TEST(oct_t_tests, FrqFrqCtor) {
	oct_t oa {frq_t{1},frq_t{2}};
	EXPECT_EQ(oa,oct_t{1});
	EXPECT_EQ(oa.to_int(),1);
	EXPECT_EQ(oa.to_double(),1.0);

	oct_t ob {frq_t{1},frq_t{4}};  // 2400 cent_t
	EXPECT_EQ(ob,oct_t{2});
	EXPECT_EQ(ob.to_int(),2);
	EXPECT_EQ(ob.to_double(),2.0);

	oct_t oc {frq_t{2},frq_t{1}};
	EXPECT_EQ(oc,oct_t{-1});
	EXPECT_EQ(oc.to_int(),-1);
	EXPECT_EQ(oc.to_double(),-1.0);

	oct_t od {frq_t{16000},frq_t{4000}};
	EXPECT_EQ(od,oct_t{-2});
	EXPECT_EQ(od.to_int(),-2);
	EXPECT_EQ(od.to_double(),-2.0);

}

//  cent_t, oct_t are consistent for the same frq ratios
TEST(oct_t_tests, OctCentFrqFrqCtorsConsistent) {
	oct_t oa {frq_t{1},frq_t{2}};  cent_t ca {frq_t{1},frq_t{2}};
	EXPECT_EQ(static_cast<int>(std::round(1200*oa.to_double())),ca.to_int());
	EXPECT_EQ(cent_t{oa},ca);

	oct_t ob {frq_t{1},frq_t{4}};  cent_t cb {frq_t{1},frq_t{4}};
	EXPECT_EQ(static_cast<int>(std::round(1200*ob.to_double())),cb.to_int());
	EXPECT_EQ(cent_t{ob},cb);

	oct_t oc {frq_t{1},frq_t{3}};  cent_t cc {frq_t{1},frq_t{3}};
	EXPECT_EQ(static_cast<int>(std::round(1200*oc.to_double())),cc.to_int());
	EXPECT_EQ(cent_t{oc},cc);
}

// Comparison operators
TEST(oct_t_tests, ComparisonOperators) {
	oct_t on1 {-1};
	oct_t oz {0};
	oct_t oz2 {0};
	oct_t op1 {1};

	EXPECT_TRUE(op1 > oz);
	EXPECT_TRUE(op1 >= oz);
	EXPECT_TRUE(op1 != oz);
	EXPECT_FALSE(op1 == oz);
	EXPECT_FALSE(op1 < oz);
	EXPECT_FALSE(op1 <= oz);

	EXPECT_TRUE(op1 > on1);
	EXPECT_TRUE(op1 >= on1);
	EXPECT_TRUE(op1 != on1);
	EXPECT_FALSE(op1 == on1);
	EXPECT_FALSE(op1 < on1);
	EXPECT_FALSE(op1 <= on1);

	EXPECT_TRUE(oz == oz2);
	EXPECT_TRUE(oz <= oz2);
	EXPECT_TRUE(oz >= oz2);
	EXPECT_FALSE(oz != oz2);
	EXPECT_FALSE(oz < oz2);
	EXPECT_FALSE(oz > oz2);

}

// +=,-=,*=,/= operators
TEST(oct_t_tests, PeMeTeDeOctCentOperators) {
	oct_t o {-1234};
	oct_t o1 {1};
	oct_t o2 {2};
	oct_t on1k {-1000};
	oct_t op3k {3000};

	o+=o1;
	EXPECT_TRUE(o == oct_t{-1233});
	o-=o2;
	EXPECT_TRUE(o == oct_t{-1235});
	o-=on1k;
	EXPECT_TRUE(o == oct_t{-235});
	o+=op3k;
	EXPECT_TRUE(o == oct_t{2765});
}


// *=int,/=int operators
TEST(oct_t_tests, TeDeOctIntOperators) {
	oct_t o1 {-1234};
	oct_t o2 {2};

	o1*=-1;
	EXPECT_TRUE(o1 == oct_t{1234});
	o1*=-1;
	EXPECT_TRUE(o1 == oct_t{-1234});
	o1/=-1;
	EXPECT_TRUE(o1 == oct_t{1234});
	o1/=-1;
	EXPECT_TRUE(o1 == oct_t{-1234});

	o2/=2;
	EXPECT_TRUE(o2 == oct_t{1});
	o2*=55;
	EXPECT_TRUE(o2 == oct_t{55});
	o2/=-55;
	EXPECT_TRUE(o2 == oct_t{-1});
	o2*=-55;
	EXPECT_TRUE(o2 == oct_t{55});
}


// oct_t/double, oct_t*double operators
TEST(oct_t_tests, OctIntTimesDivideOperators) {
	oct_t o1 {-3};
	oct_t oresult {0};
	oct_t test {3.0/51.0};

	oresult = o1/-1.0;
	EXPECT_TRUE(oresult == oct_t{3});
	oresult = oresult*(-1.0);
	EXPECT_TRUE(oresult == oct_t{-3});
	oresult = oresult/-1.0;
	EXPECT_TRUE(oresult == oct_t{3});
	oresult = o1/51.0;
	EXPECT_TRUE(oresult == oct_t{-3.0/51.0});
	oresult = o1/54.0;
	EXPECT_TRUE(oresult == oct_t{-3.0/54.0});
}

