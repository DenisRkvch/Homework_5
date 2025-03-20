#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "../SpaceBattle/IoCContainer/Scope.h"
#include "../SpaceBattle/IoCContainer/IScope.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IFunctor.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"
#include <map>

using ::testing::Return;
using ::testing::Throw;

class ScopeMock : public IScope {
public:

    MOCK_METHOD((const std::string&), getId, (), (override));
    MOCK_METHOD((void), add, (const std::string&, IFunctor*), (override));
    MOCK_METHOD((IScope*), parent, (), (override));
    MOCK_METHOD((void), newChild, (const std::string&), (override));
    MOCK_METHOD((void), deleteChild, (const std::string&), (override));
    MOCK_METHOD((IScope*), getChild, (const std::string&), (override));

    IFunctor *operator [](const std::string& str) override{
         return operator_brackets(str);
    }
    MOCK_METHOD((IFunctor*), operator_brackets, (const std::string&), ());
};


class FunctorMock : public IFunctor{
public:
    ~FunctorMock() override { destructor(); }	// дл¤ проверки вызова деструктора
    MOCK_METHOD((void), destructor, ());
};


TEST(TestScope, construct_and_resolve_dependency)
{
    std::map<std::string, IFunctor*> depMap;
    IFunctor* f = new IFunctor;

    depMap["key"] = f;

    EXPECT_NO_THROW({
        Scope testScope("id", depMap);
        EXPECT_EQ(testScope["key"], f);
    });
}

TEST(TestScope, add_and_resolve_dependency)
{
    IFunctor* f = new IFunctor;

    EXPECT_NO_THROW({
        Scope testScope("id");
        testScope.add("key", f);
        EXPECT_EQ(testScope["key"], f);
    });
}

TEST(TestScope, dependency_not_found_exception)
{
    Scope testScope("id");

    EXPECT_THROW({
        IFunctor* f = testScope["some_key"];
    }, IoCException);
}

TEST(TestScope, deleting_scope_destructs_dependencys)
{
    FunctorMock* fMock = new FunctorMock;
    FunctorMock* fMock1 = new FunctorMock;

    Scope* testScope = new Scope("id", std::map<std::string, IFunctor*>{{"key", fMock}, {"key1", fMock1}});

    EXPECT_CALL(*fMock, destructor()).Times(1);
    EXPECT_CALL(*fMock1, destructor()).Times(1);

    EXPECT_NO_THROW({
        delete testScope;
    });
}

TEST(TestScope, get_scope_id)
{
    Scope testScope("ID:test");

    EXPECT_NO_THROW({
        EXPECT_EQ(testScope.getId(), std::string("ID:test"));
    });
}

TEST(TestScope, get_parent_scope)
{
    ScopeMock parentMock;
    Scope testScope("id", &parentMock);

    EXPECT_NO_THROW({
        EXPECT_EQ(testScope.parent(), &parentMock);
    });
}

TEST(TestScope, parent)
{
    ScopeMock parentMock;
    Scope testScope("id", &parentMock);

    EXPECT_NO_THROW({
        EXPECT_EQ(testScope.parent(), &parentMock);
    });
}

TEST(TestScope, get_dependency_from_parent_parent)
{
    ScopeMock parent_parentMock;
    Scope testParentScope("middle", &parent_parentMock);
    Scope testScope("last", &testParentScope);
    IFunctor fMock;

    EXPECT_CALL(parent_parentMock, operator_brackets("some_key")).Times(1).WillOnce(Return(&fMock));

    EXPECT_NO_THROW({
        IFunctor* f = testScope["some_key"];
        EXPECT_EQ(f, &fMock);
    });
}

TEST(TestScope, dependency_not_found_from_parent_exception)
{
    ScopeMock parentMock;
    Scope testScope("id", &parentMock);

    EXPECT_CALL(parentMock, operator_brackets("some_key")).Times(1).WillOnce(Throw(IoCException()));

    EXPECT_THROW({
        IFunctor* f = testScope["some_key"];
    }, IoCException);
}

TEST(TestScope, create_and_get_child_scope)
{
    Scope root_scope("root");

    EXPECT_NO_THROW({
        root_scope.newChild("child_id");
        IScope* child_scope = root_scope.getChild("child_id");
        EXPECT_EQ(child_scope->getId(), "child_id");
    });

}

TEST(TestScope, try_to_get_not_existing_child)
{
    Scope root_scope("id");
   
    EXPECT_THROW({
    IScope* child_scope = root_scope.getChild("child_id");
    }, IoCException);
}

TEST(TestScope, try_to_create_child_with_parent_id)
{
    Scope root_scope("root");

    EXPECT_THROW({
        root_scope.newChild("root");
    }, IoCException);
}

TEST(TestScope, try_to_create_two_children_with_same_id)
{
    Scope root_scope("root");

    EXPECT_THROW({
        root_scope.newChild("child");
        root_scope.newChild("child");
    }, IoCException);
}

TEST(TestScope, delete_child)
{
    Scope root_scope("root");

    root_scope.newChild("child");

    EXPECT_NO_THROW({
        IScope * child_scope = root_scope.getChild("child");
    });

    root_scope.deleteChild("child");

    EXPECT_THROW({
        IScope* child_scope = root_scope.getChild("child");
    }, IoCException);
}

TEST(TestScope, try_to_delete_no_existing_child)
{
    Scope root_scope("root");

    EXPECT_THROW({
        root_scope.deleteChild("child");
    }, IoCException);
}
