#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;

#include "../SpaceBattle/IoCContainer/IoCConnectContainerCommand.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoC.h"


class IoCContainerMock : public IIoCContainer {
public:
	MOCK_METHOD((IFunctor*), brackets_operator, (const std::string&));

	IFunctor* operator [](const std::string& str) override
	{
		return brackets_operator(str);
	}
};



TEST(TestIoCConnectContainerComamnd, connect_succeed)
{
	IoCContainerMock mock_container;
	IFunctor* return_dummy = New_Resolver(static_cast<void(*)()>([](){}));

	IoCConnectContainerCommand((IIoCContainer*)&mock_container).execute();

	EXPECT_CALL(mock_container, brackets_operator("TestStr"))
		.Times(1)
		.WillOnce(Return(return_dummy));

	IoC::Resolve<void>("TestStr");
}

TEST(TestIoCConnectContainerComamnd, reconnect_succeed)
{
	IoCContainerMock mock_container_fst;
	IoCContainerMock mock_container_snd;
	IFunctor* return_dummy = New_Resolver(static_cast<void(*)()>([]() {}));

	EXPECT_CALL(mock_container_fst, brackets_operator("TestStrFst"))
		.Times(1)
		.WillOnce(Return(return_dummy));

	EXPECT_CALL(mock_container_snd, brackets_operator("TestStrSnd"))
		.Times(1)
		.WillOnce(Return(return_dummy));

	IoCConnectContainerCommand((IIoCContainer*)&mock_container_fst).execute();
	IoC::Resolve<void>("TestStrFst");

	IoCConnectContainerCommand((IIoCContainer*)&mock_container_snd).execute();
	IoC::Resolve<void>("TestStrSnd");
}
