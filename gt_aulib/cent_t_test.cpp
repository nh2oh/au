#include "gtest/gtest.h"
#include "..\aulib\types\cent_oct_t.h"
#include "..\aulib\types\frq_t.h"


// Default ctor
TEST(cent_t_tests, DefaultCtor) {
	cent_t c {};
	EXPECT_EQ(c.to_int(),0);
	EXPECT_EQ(c,cent_t{0});
}

// Ctor frq ratios
TEST(cent_t_tests, FrqFrqCtor) {
	cent_t ca {frq_t{1},frq_t{2}};
	EXPECT_EQ(ca,cent_t{1200});

	cent_t cb {frq_t{1},frq_t{3}};  // 1901.96
	EXPECT_EQ(cb,cent_t{1902});

	cent_t cc {frq_t{2},frq_t{1}};
	EXPECT_EQ(cc,cent_t{-1200});

	cent_t cd {frq_t{38979},frq_t{711}};  // -6932.04
	EXPECT_EQ(cd,cent_t{-6932});

	cent_t ce {frq_t{2},frq_t{3}};  // 701.955
	EXPECT_EQ(ce,cent_t{702});
}

// Comparison operators
TEST(cent_t_tests, ComparisonOperators) {
	cent_t cn1 {-1};
	cent_t cz {0};
	cent_t cz2 {0};
	cent_t cp1 {1};

	EXPECT_TRUE(cp1 > cz);
	EXPECT_TRUE(cp1 >= cz);
	EXPECT_TRUE(cp1 != cz);
	EXPECT_FALSE(cp1 == cz);
	EXPECT_FALSE(cp1 < cz);
	EXPECT_FALSE(cp1 <= cz);

	EXPECT_TRUE(cp1 > cn1);
	EXPECT_TRUE(cp1 >= cn1);
	EXPECT_TRUE(cp1 != cn1);
	EXPECT_FALSE(cp1 == cn1);
	EXPECT_FALSE(cp1 < cn1);
	EXPECT_FALSE(cp1 <= cn1);

	EXPECT_TRUE(cz == cz2);
	EXPECT_TRUE(cz <= cz2);
	EXPECT_TRUE(cz >= cz2);
	EXPECT_FALSE(cz != cz2);
	EXPECT_FALSE(cz < cz2);
	EXPECT_FALSE(cz > cz2);

}

// +=,-=,*=,/= operators
TEST(cent_t_tests, PeMeTeDeCentCentOperators) {
	cent_t c {-1234};
	cent_t c1 {1};
	cent_t c2 {2};
	cent_t cn1k {-1000};
	cent_t cp3k {3000};

	c+=c1;
	EXPECT_TRUE(c == cent_t{-1233});
	c-=c2;
	EXPECT_TRUE(c == cent_t{-1235});
	c-=cn1k;
	EXPECT_TRUE(c == cent_t{-235});
	c+=cp3k;
	EXPECT_TRUE(c == cent_t{2765});
}


// *=int,/=int operators
TEST(cent_t_tests, TeDeCentIntOperators) {
	cent_t c1 {-1234};
	cent_t c2 {2};

	c1*=-1;
	EXPECT_TRUE(c1 == cent_t{1234});
	c1*=-1;
	EXPECT_TRUE(c1 == cent_t{-1234});
	c1/=-1;
	EXPECT_TRUE(c1 == cent_t{1234});
	c1/=-1;
	EXPECT_TRUE(c1 == cent_t{-1234});

	c2/=2;
	EXPECT_TRUE(c2 == cent_t{1});
	c2*=55;
	EXPECT_TRUE(c2 == cent_t{55});
	c2/=-55;
	EXPECT_TRUE(c2 == cent_t{-1});
	c2*=-55;
	EXPECT_TRUE(c2 == cent_t{55});
}


// cent_t/int, cent_t*int operators
TEST(cent_t_tests, CentIntTimesDivideOperators) {
	cent_t c1 {-1234};
	cent_t cresult {0};

	cresult = c1/-1;
	EXPECT_TRUE(cresult == cent_t{1234});
	cresult = cresult*(-1);
	EXPECT_TRUE(cresult == cent_t{-1234});
	cresult = cresult/-1;
	EXPECT_TRUE(cresult == cent_t{1234});
	cresult = c1/51;
	EXPECT_TRUE(cresult == cent_t{-24});
	cresult = c1/54;
	EXPECT_TRUE(cresult == cent_t{-22});
}








