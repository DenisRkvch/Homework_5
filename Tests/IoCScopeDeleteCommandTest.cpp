#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../SpaceBattle/IoCContainer/Commands/IoCScopeDeleteCommand.h"
#include "../SpaceBattle/IoCContainer/Scope.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"

using testing::Return;
using testing::ReturnRef;
class ScopeMock : public IScope{
public:

    MOCK_METHOD((const std::string&), getId, (), (override));
    MOCK_METHOD((void), add, (const std::string&, IFunctor*), (override));
    MOCK_METHOD((IScope*), parent, (), (override));
    MOCK_METHOD((void), newChild, (const std::string&), (override));
    MOCK_METHOD((void), deleteChild, (const std::string&), (override));
    MOCK_METHOD((IScope*), getChild, (const std::string&), (override));

    IFunctor* operator [](const std::string& str) override {return operator_brackets(str); }
    MOCK_METHOD((IFunctor*), operator_brackets, (const std::string&), ());
};

TEST(TestIoCScopeDeleteCommand, delete_current_scope_succeed)
{
    ScopeMock root_scopeMock;
    ScopeMock current_scopeMock;
    std::string str = "id";

    EXPECT_CALL(current_scopeMock, parent()).Times(1).WillOnce(Return(&root_scopeMock));
    EXPECT_CALL(current_scopeMock, getId()).Times(1).WillOnce(ReturnRef(str));
    EXPECT_CALL(root_scopeMock, deleteChild("id")).Times(1);
    EXPECT_NO_THROW({
        IoCScopeDeleteCommand testCommand(new IScope*(&current_scopeMock));
        testCommand.execute();
    });
}

TEST(TestIoCScopeDeleteCommand, delete_current_scope_faild)
{
    ScopeMock current_scopeMock;

    EXPECT_CALL(current_scopeMock, parent()).Times(1).WillOnce(Return(nullptr));
    
    EXPECT_THROW({
        IoCScopeDeleteCommand testCommand(new IScope*(&current_scopeMock));
        testCommand.execute();
    }, IoCException);

}

TEST(TestIoCScopeDeleteCommand, change_current_scope_to_root_on_delete)
{
    Scope root_scope("root_scope");
    root_scope.newChild("sub_scope");
    IScope* currScopePtr = root_scope.getChild("sub_scope");

    IoCScopeDeleteCommand testCommand(&currScopePtr);
    testCommand.execute();

    EXPECT_EQ(currScopePtr, &root_scope);
}
