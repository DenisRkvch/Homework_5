#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Throw;
using ::testing::Return;

#include "../SpaceBattle/IoCContainer/IoCCore/IoC.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"
#include "../SpaceBattle/IoCContainer/IoCConnectContainerCommand.h"

class IoCContainerMock : public IIoCContainer {
public:
	MOCK_METHOD((IFunctor*), brackets_operator, (const std::string&));

	IFunctor* operator [](const std::string& str) override
	{
		return brackets_operator(str);
	}
};


TEST(TestIoC, wrapping_function_by_using__New_Resolver1)
{
    std::function<const char*(const char*)> function = [](const char* str){
        return str;
    };
    IFunctor* WrappFunc = New_Resolver(function);
    const char* out;

    EXPECT_NO_THROW({
        out = WrappFunc->call<const char*>("test str");
    });

    EXPECT_STREQ(out , "test str");
}

TEST(TestIoC, wrapping_function_by_using__New_Resolver2)
{
    const char*(*regularFunction)(const char*) = [](const char* in){return in;};
    IFunctor* WrappFunc = New_Resolver(regularFunction);
    const char* out;

    EXPECT_NO_THROW({
        out = WrappFunc->call<const char*>("test str");
    });

    EXPECT_STREQ(out , "test str");
}


TEST(TestIoC, dependancy_solve)
{
    IoCContainerMock mock_container;
    IoCConnectContainerCommand((IIoCContainer*)&mock_container).execute();

    EXPECT_CALL(mock_container, brackets_operator("key"))
        .Times(1)
        .WillOnce(Return(New_Resolver(static_cast<const char*(*)(const char*)>([](const char* in){
            return in;
        }))));

    const char* out;

    EXPECT_NO_THROW({
        out = IoC::Resolve<const char*>("key", "test str");
    });

    EXPECT_STREQ(out , "test str");
}

TEST(TestIoC, dependancy_not_found_exception)
{
	IoCContainerMock mock_container;

	IoCConnectContainerCommand((IIoCContainer*)&mock_container).execute();

	EXPECT_CALL(mock_container, brackets_operator("TestStr"))
		.Times(1)
		.WillOnce(Throw(IoCException("dependancy_not_found")));

	EXPECT_THROW({
		IoC::Resolve<void>("TestStr");
	}, IoCException);
}
