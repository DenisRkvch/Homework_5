#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <functional>

#include "../SpaceBattle/IoCContainer/IoCCore/IFunctor.h"

using ::testing::Return;

class TestMock{
public:
	MOCK_METHOD((const char*), mock_function, (const char*), ());
};

TEST(TestFunctor, call_wrapped_function)
{
	TestMock mock;
	const char* ret;
	std::function<const char*(const char*)> f = [&mock](const char* str) -> const char*{
		return mock.mock_function(str);
	};
	
	Functor functor(f);
	
	EXPECT_CALL(mock, mock_function("in")).Times(1).WillOnce(Return("Out"));
	
	ret = functor("in");
	
	EXPECT_STREQ(ret, "Out");
}

TEST(TestFunctor, polymorphic_call_wrapped_function2)
{
	TestMock mock;
	const char* ret;
	std::function<const char*(const char*)> f = [&mock](const char* str) -> const char*{
		return mock.mock_function(str);
	};
	
	IFunctor* functor_ptr = new Functor(f);
	
	EXPECT_CALL(mock, mock_function("in")).Times(1).WillOnce(Return("Out"));
	
	ret = functor_ptr->call<const char*, const char*>("in");
	
	EXPECT_STREQ(ret, "Out");
}

