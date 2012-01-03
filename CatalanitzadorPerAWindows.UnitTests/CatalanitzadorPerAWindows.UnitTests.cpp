// CatalanitzadorPerAWindows.UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"


int _tmain(int argc, _TCHAR* argv[])
{
	::testing::InitGoogleMock(&argc, argv);
	::testing::InitGoogleTest(&argc, argv);	 
	RUN_ALL_TESTS();
	return 0;
}