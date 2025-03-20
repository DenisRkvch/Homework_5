#include <gtest/gtest.h>

#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"

TEST(TestIoCException, check_exception_message)
{
	EXPECT_NO_THROW(
	{
		try{
			throw IoCException("test_msg");			
		}catch(std::exception& e)
		{
			EXPECT_STREQ(e.what(), "IoC Exception: test_msg");
		}
	});
}


