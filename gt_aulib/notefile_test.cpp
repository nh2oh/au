#include "gtest/gtest.h"
#include "..\aulib\input\notefile.h"
#include <string>
#include <vector>
#include <filesystem>

TEST(notefile_tests, file_nonexistent_tests) {
	std::string file = "..\\stuff\\whatever\\deutblanhblah1334.notes";

	auto nf = read_notefile(file);
	bool tf {false};
	tf = (nf.file_error==true); EXPECT_TRUE(tf);
	tf = (nf.fname == ""); EXPECT_TRUE(tf);
	tf = (nf.fpath == ""); EXPECT_TRUE(tf);
	tf = nf.error_lines.empty(); EXPECT_TRUE(tf);
	tf = nf.lines.empty(); EXPECT_TRUE(tf);
	tf = nf.nonnote_lines.empty(); EXPECT_TRUE(tf);
}

TEST(notefile_tests, deut1334_tests) {
	std::string file {"..\\..\\stuff\\deut1334.notes"};
	auto nf = read_notefile(file);
	bool tf {false};

	std::string ans_nonnote_lines = "Meta event, unrecognized, type=0x20 leng=1";
		// The 3 non-note lines in this file are identical and == to this
	std::vector<int> ans_nonnote_file_line_num = {1,2,3};

	std::vector<int> ans_dt_ms = {595,544,291,254,168,329,538,259,277,
		279,294,349,308,315,554,263,318,600,577,1123,172,277,257,
		281,282,344,598,637,1116};
	std::vector<int> ans_ontime_ms = {0,554,1103,1382,1644,1928,2212,2738,3010,
		3294,3564,3865,4417,4701,4977,5520,5785,6080,6651,7197,8875,9150,9420,
		9691,9960,10232,10528,11092,11708};
	std::vector<int> ans_offtime_ms = {595,1098,1394,1636,1812,2257,2750,2997,
		3287,3573,3858,4214,4725,5016,5531,5783,6103,6680,7228,8320,9047,9427,
		9677,9972,10242,10576,11126,11729,12824};
	std::vector<int> ans_pitch = {67,72,71,69,67,67,72,67,64,65,67,64,65,62,
		64,65,62,64,62,60,72,72,71,74,72,69,71,69,67};
	int ans_line_num_first_note_line = 4;

	EXPECT_FALSE(nf.file_error);
	EXPECT_TRUE(nf.fname == "deut1334.notes");
	EXPECT_EQ(nf.fpath,"..\\..\\stuff");
	EXPECT_TRUE(nf.error_lines.empty());
	EXPECT_TRUE(nf.lines.size()==29);
	EXPECT_TRUE(nf.nonnote_lines.size()==3);

	for (int i=0; i<nf.nonnote_lines.size(); ++i) {
		EXPECT_EQ(nf.nonnote_lines[i].file_line_num,ans_nonnote_file_line_num[i]);
		EXPECT_EQ(nf.nonnote_lines[i].linedata,ans_nonnote_lines);
	}

	for (int i=0; i<nf.lines.size(); ++i) {
		EXPECT_EQ(static_cast<int>(nf.lines[i].dt),ans_dt_ms[i]);
		EXPECT_EQ(static_cast<int>(nf.lines[i].ontime),ans_ontime_ms[i]);
		EXPECT_EQ(static_cast<int>(nf.lines[i].offtime),ans_offtime_ms[i]);
		EXPECT_EQ(nf.lines[i].pitch,ans_pitch[i]);
		EXPECT_EQ(nf.lines[i].file_line_num,(ans_line_num_first_note_line+i));
	}
}

TEST(notefile_tests, deut1334_readwriteread_tests) {
	std::string file {"..\\..\\stuff\\deut1334.notes"};
	auto nf_read1 = read_notefile(file);
	auto nf_read1_write = nf_read1;
	nf_read1_write.fname = "deut1334writetest.notes";
	nf_read1_write.fpath = ".\\autest_temp\\";
	std::filesystem::create_directory(std::filesystem::path {".\\autest_temp\\"});
	auto tf_write = write_notefile(nf_read1_write);
	EXPECT_TRUE(tf_write);

	auto nf_read2 = read_notefile(nf_read1_write.fpath + nf_read1_write.fname);

	nf_read2.fname = nf_read1.fname;
	nf_read2.fpath = nf_read1.fpath;
	EXPECT_EQ(nf_read2,nf_read1);

	std::filesystem::remove_all(std::filesystem::path {".\\autest_temp\\"});
}

