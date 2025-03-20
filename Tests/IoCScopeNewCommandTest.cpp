#include <gtest/gtest.h>

#include "../SpaceBattle/IoCContainer/Commands/IoCScopeNewCommand.h"
#include "../SpaceBattle/IoCContainer/Scope.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"


TEST(TestIoCScopeNewCommand, create_new_scope_succeed)
{
    Scope scope("scope");

    EXPECT_NO_THROW({
        IoCScopeNewCommand testCommand("new", &scope);
        testCommand.execute();
        IScope* newScope = scope.getChild("new");
        EXPECT_EQ(newScope->getId(), "new");
    });
}

TEST(TestIoCScopeNewCommand, create_new_scope_failed)
{
    Scope scope("scope");
    IoCScopeNewCommand testCommand("scope", &scope);

    EXPECT_THROW({
        testCommand.execute();
    }, IoCException);
}

TEST(TestIoCScopeNewCommand, create_new_scope_failed2)
{
    Scope scope("scope");
    IoCScopeNewCommand testCommand("new", &scope);
    IoCScopeNewCommand testCommand2("new", &scope);

    EXPECT_NO_THROW({
        testCommand.execute();
    });

    EXPECT_THROW({
        testCommand2.execute();
    }, IoCException);
}
