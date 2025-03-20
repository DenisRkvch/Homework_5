#include <gtest/gtest.h>

#include "../SpaceBattle/IoCContainer/Commands/IoCRegisterCommand.h"
#include "../SpaceBattle/IoCContainer/Scope.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"


TEST(TestIoCRegisterCommand, register_succeed)
{
    Scope scope("scope");
    IFunctor* f = new IFunctor;

    EXPECT_NO_THROW({
        IoCRegisterCommand testCommand(&scope, "key", f);
        testCommand.execute();
        EXPECT_EQ(scope["key"], f);
    });
}

TEST(TestIoCRegisterCommand, register_failed)
{
    Scope scope("scope");
    IFunctor* f = new IFunctor;
    IoCRegisterCommand testCommand(&scope, "key", f);

    EXPECT_NO_THROW({
        testCommand.execute();
    });
    EXPECT_THROW({
        testCommand.execute();
    }, IoCException);
}
