#include "gtest/gtest.h"
#include "..\aulib\melgen\randmel_gens.h"
#include "..\aulib\types\line_t.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\types\ts_t.h"
#include "..\aulib\types\beat_bar_t.h"
#include "..\aulib\types\nv_t.h"

// ...
TEST(melody_temperley_tests, yay) {


	ntl_t keyntl {"C"};
	bool ismajor {true};
	ts_t ts {4_bt,d::q};
	bar_t nbars {4};
	line_t<note_t> rm {};  // "random melody"
	rm = melody_temperley(keyntl, ismajor, ts, nbars);

	bool tf {false};
	tf = (rm.nbars()==nbars);
	EXPECT_TRUE(tf);

}



