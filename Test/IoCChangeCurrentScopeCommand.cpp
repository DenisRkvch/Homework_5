#include <gtest/gtest.h>

#include "../SpaceBattle/IoCContainer/Commands/IoCChangeCurrentScopeCommand.h"
#include "../SpaceBattle/IoCContainer/Scope.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"


TEST(TestIoCChangeCurrentScopeCommand, change_to_child_succeed)
{
    Scope scope("root");
    IScope* currScopePtr = &scope;
    scope.newChild("subScope");

    EXPECT_NO_THROW({
        IoCChangeCurrentScopeCommand testCommand("subScope", &currScopePtr);
        testCommand.execute();
        EXPECT_EQ(currScopePtr->getId(), "subScope");
    });
}


TEST(TestIoCChangeCurrentScopeCommand, change_to_self_succeed)
{
    Scope scope("root");
    IScope* currScopePtr = &scope;

    EXPECT_NO_THROW({
        IoCChangeCurrentScopeCommand testCommand("root", &currScopePtr);
        testCommand.execute();
        EXPECT_EQ(currScopePtr->getId(), "root");
    });
}

TEST(TestIoCChangeCurrentScopeCommand, change_to_parent_succeed)
{
    Scope scope("root");
    scope.newChild("subScope");
    IScope* currScopePtr = scope.getChild("subScope");

    EXPECT_NO_THROW({
        EXPECT_EQ(currScopePtr->getId(), "subScope");

        IoCChangeCurrentScopeCommand testCommand("root", &currScopePtr);
        testCommand.execute();

        EXPECT_EQ(currScopePtr->getId(), "root");
    });
}

TEST(TestIoCChangeCurrentScopeCommand, change_to_parents_child_succeed)
{
    Scope scope("root");
    scope.newChild("subScope1");
    scope.newChild("subScope2");
    IScope* currScopePtr = scope.getChild("subScope1");

    EXPECT_NO_THROW({
        EXPECT_EQ(currScopePtr->getId(), "subScope1");

        IoCChangeCurrentScopeCommand testCommand("subScope2", &currScopePtr);
        testCommand.execute();

        EXPECT_EQ(currScopePtr->getId(), "subScope2");
    });
}

TEST(TestIoCChangeCurrentScopeCommand, change_to_nothing_failed)
{
    Scope scope("root");
    IScope* currScopePtr = &scope;

    EXPECT_THROW({
        IoCChangeCurrentScopeCommand testCommand("subScope", &currScopePtr);
        testCommand.execute();
    }, IoCException);
}
