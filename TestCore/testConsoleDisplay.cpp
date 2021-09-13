#include "pch.h"
#include "../Core/ConsoleDisplay.h"

TEST(color2RGBcodeTest, CheckInOut) {
	EXPECT_EQ(color2RGBcode(colorRGB(255,255,255)), 0xFFFFFF);
	EXPECT_EQ(color2RGBcode(colorRGB(0, 0, 0)), 0x000000);
	EXPECT_EQ(color2RGBcode(colorRGB(0, 1, 0)), 0x000100);
	EXPECT_EQ(color2RGBcode(colorRGB(0, 128, 0)), 0x008000);
}