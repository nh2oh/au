#include "mtrk_test_data.h"
#include <vector>
#include <cstdint>


namespace mtrk_tests {

extern mtrk_properties_t tsa_props {31,2688};  // 31 events in tsa

std::vector<eventdata_with_cumtk_t> tsa {
	// meta instname: "acoustic grand";
	{{0x00u,0xFFu,0x04u,0x0Eu,0x61u,0x63u,0x6Fu,0x75u,0x73u,0x74u,0x69u,
		0x63u,0x20u,0x67u,0x72u,0x61u,0x6Eu,0x64u}, 0},  
	{{0x00u,0xB0u,0x07u,0x64u}, 0},
	{{0x00u,0xFFu,0x59u,0x02u,0x00u,0x00u}, 0},  // Key sig

	{{0x00u,0x90u,0x48u,0x5Au}, 0},
	{{0x00u,0x90u,0x3Cu,0x5Au}, 0},
	{{0x83u,0x00u,0x90u,0x48u,0x00u}, 0},  // idx==5, delta_time()==384, cumtk == 0

	{{0x00u,0x90u,0x3Cu,0x00u}, 384},  // cumtk==384, event idx == 6
	{{0x00u,0x90u,0x37u,0x5Au}, 384},
	{{0x00u,0x90u,0x45u,0x5Au}, 384},
	{{0x83u,0x00u,0x90u,0x37u,0x00u}, 384},  // cumtk==384, idx==9, , delta_time()==384

	{{0x00u,0x90u,0x45u,0x00u}, 768},  // cumtk==768, idx==10
	{{0x00u,0x90u,0x47u,0x5Au}, 768},
	{{0x83u,0x00u,0x90u,0x47u,0x00u}, 768},

	{{0x00u,0x90u,0x41u,0x5Au}, 1152},
	{{0x00u,0x90u,0x34u,0x5Au}, 1152},
	{{0x83u,0x00u,0x90u,0x41u,0x00u}, 1152},
	{{0x00u,0x90u,0x34u,0x00u}, 1536},
	{{0x00u,0x90u,0x41u,0x5Au}, 1536},
	{{0x00u,0x90u,0x34u,0x5Au}, 1536},
	{{0x83u,0x00u,0x90u,0x41u,0x00u}, 1536},
	{{0x00u,0x90u,0x34u,0x00u}, 1920},
	{{0x00u,0x90u,0x3Cu,0x5Au}, 1920},
	{{0x00u,0x90u,0x30u,0x5Au}, 1920},
	{{0x83u,0x00u,0x90u,0x3Cu,0x00u}, 1920},
	{{0x00u,0x90u,0x30u,0x00u}, 2304},  // idx==24
	{{0x00u,0x90u,0x3Cu,0x5Au}, 2304},
	{{0x83u,0x00u,0x90u,0x3Cu,0x00u}, 2304},  // idx==26
	{{0x00u,0x90u,0x34u,0x5Au}, 2688},
	{{0x00u,0x90u,0x3Cu,0x5Au}, 2688},
	{{0x00u,0x90u,0x41u,0x5Au}, 2688},
	{{0x00u,0xFFu,0x2Fu,0x00u}, 2688}  // EOT; idx==30
};



std::vector<tsb_t> tsb {
	// meta instname: "acoustic grand";
	{{0x00u,0xFFu,0x04u,0x0Eu,0x61u,0x63u,0x6Fu,0x75u,0x73u,0x74u,0x69u,
		0x63u,0x20u,0x67u,0x72u,0x61u,0x6Eu,0x64u}, 0, 0},  
	{{0x00u,0xB0u,0x07u,0x64u}, 0, 0},
	{{0x00u,0xFFu,0x59u,0x02u,0x00u,0x00u}, 0, 0},  // Key sig

	{{0x05u,0x90u,0x48u,0x59u}, 0, 5},
	{{0x81u,0x05u,0x80u,0x48u,0x17u}, 5, 138},

	{{0x07u,0x90u,0x4Cu,0x5Fu}, 138, 145},
	{{0x43u,0x90u,0x48u,0x58u}, 145, 212},
	{{0x10u,0x80u,0x4Cu,0x0Cu}, 212, 228},
	{{0x32u,0x80u,0x48u,0x12u}, 228, 278},

	{{0x00u,0x90u,0x43u,0x58u}, 278, 278},  // dt==0
	{{0x1Bu,0x80u,0x43u,0x1Cu}, 278, 305},  // dt==27

	{{0x69u,0x90u,0x43u,0x5Fu}, 305, 410},  // dt==105
	{{0x21u,0x80u,0x43u,0x23u}, 410, 443},  // dt==33

	{{0x6Eu,0x90u,0x48u,0x5Eu}, 443, 553},
	{{0x71u,0x80u,0x48u,0x1Bu}, 553, 666},

	{{0x07u,0x90u,0x4Cu,0x65u}, 666, 673},
	{{0x43u,0x90u,0x48u,0x64u}, 673, 740},

	{{0x06u,0x80u,0x4Cu,0x16u}, 740, 746},
	{{0x32u,0x80u,0x48u,0x20u}, 746, 796},

	{{0x0Cu,0x90u,0x43u,0x65u}, 796, 808},  // dt==12
	{{0x23u,0x80u,0x43u,0x25u}, 808, 843},  // dt==35

	{{0x67u,0x90u,0x4Fu,0x66u}, 843, 946},
	{{0x22u,0x80u,0x4Fu,0x1Au}, 946, 980},

	{{0x63u,0x90u,0x4Du,0x63u}, 980, 1079},
	{{0x3Du,0x90u,0x4Cu,0x60u}, 1079, 1140},
	{{0x15u,0x80u,0x4Du,0x18u}, 1140, 1161},
	{{0x26u,0x90u,0x4Au,0x63u}, 1161, 1199},
	{{0x18u,0x80u,0x4Cu,0x0Fu}, 1199, 1223},
	{{0x1Cu,0x80u,0x4Au,0x23u}, 1223, 1251},

	{{0x00u,0xFFu,0x2Fu,0x00u}, 1251, 1251}  // EOT; idx==
};  // std::vector<> tsb

// Note that the raw data for these events still have delta time fields from
// the original mtrk.  
std::vector<tsb_t> tsb_note_67_events {
	{{0x00u,0x90u,0x43u,0x58u}, 278, 278},
	{{0x1Bu,0x80u,0x43u,0x1Cu}, 278, 305},

	{{0x69u,0x90u,0x43u,0x5Fu}, 305, 410},
	{{0x21u,0x80u,0x43u,0x23u}, 410, 443},

	{{0x0Cu,0x90u,0x43u,0x65u}, 796, 808},
	{{0x23u,0x80u,0x43u,0x25u}, 808, 843}
};

// Note that the raw data for these events still have delta time fields from
// the original mtrk.  
std::vector<tsb_t> tsb_meta_events {
	{{0x00u,0xFFu,0x04u,0x0Eu,0x61u,0x63u,0x6Fu,0x75u,0x73u,0x74u,0x69u,
		0x63u,0x20u,0x67u,0x72u,0x61u,0x6Eu,0x64u}, 0, 0},  
	{{0x00u,0xFFu,0x59u,0x02u,0x00u,0x00u}, 0, 0},  // Key sig
	{{0x00u,0xFFu,0x2Fu,0x00u}, 1251, 1251}  // EOT
};

};  // namespace mtrk_tests

