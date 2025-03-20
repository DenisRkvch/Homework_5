#include <gtest/gtest.h>
#include <string>
#include <thread>

#include "../SpaceBattle/IoCContainer/IoCConnectContainerCommand.h"
#include "../SpaceBattle/IoCContainer/ScopeTree.h"
#include "../SpaceBattle/IoCContainer/IoCCore/IoCException.h"


TEST(TestScopeTree, get_current_scope_id)
{
    EXPECT_NO_THROW({
        const std::string& curScopeId = ScopeTree::getCurScopeId();
        EXPECT_EQ(curScopeId, "Root");
    });
}
// Root
// ^
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Current__set_same_scope)
{
    IoCConnectContainerCommand((IIoCContainer*)&ScopeTree::g_scopeTree).execute();

    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("Root"))->execute();
    });
    EXPECT_EQ(ScopeTree::getCurScopeId(), "Root");
}
// Root
// ^
TEST(TestScopeTree_basic_dependencys, IoC_Scope_New__create_2new_scopes)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope1"))->execute();
        IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope2"))->execute();
    });
}
// Root - sub_scope1
// ^    \
//        sub_scope2
TEST(TestScopeTree_basic_dependencys, IoC_Scope_New__create_same_scope_fail)
{
    EXPECT_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope1"))->execute();
    }, IoCException);
}
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Current__set_sub_scope)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
    });
    EXPECT_EQ(ScopeTree::getCurScopeId(), "sub_scope1");
}
// Root - sub_scope1
//      \ ^
//        sub_scope2
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Current__set_neighbor_scope)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope2"))->execute();
    });
    EXPECT_EQ(ScopeTree::getCurScopeId(), "sub_scope2");

    IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope2_1"))->execute();
}
// Root - sub_scope1
//      \
//        sub_scope2 - sub_scope2-1
//        ^
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Delete__delete_scope)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Delete")->execute();
    });
    EXPECT_EQ(ScopeTree::getCurScopeId(), "Root");
}
// Root - sub_scope1
// ^
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Current__set_noexisting_scope_fail)
{
    EXPECT_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope2"))->execute();
    }, IoCException);
}
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Delete__delete_root_fail)
{
    EXPECT_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Delete")->execute();
    }, IoCException);
}
TEST(TestScopeTree_basic_dependencys, IoC_Register__register_new_dependency)
{
    IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();

    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>(
                    "IoC.Register",
                    std::string("logic_not"),
                    New_Resolver(static_cast<bool(*)(bool)>([](bool b){
                        return !b;
                    })))->execute();
    });
}
// Root - sub_scope1
//        ^
TEST(TestScopeTree_basic_dependencys, IoC_Register__solve_dependency)
{
    EXPECT_NO_THROW({
        EXPECT_EQ(IoC::Resolve<bool>("logic_not", true), false);
    });
}
TEST(TestScopeTree_basic_dependencys, IoC_Register__register_same_dependency_fail)
{
    EXPECT_THROW({
        IoC::Resolve<ICommand*>(
                    "IoC.Register",
                    std::string("logic_not"),
                    New_Resolver(static_cast<bool(*)(bool)>([](bool b){
                        return !b;
                    })))->execute();
    }, IoCException);
}
TEST(TestScopeTree_basic_dependencys, IoC_Register__dependency_visible_from_child_scope)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope1_1"))->execute();
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_1"))->execute();
        EXPECT_EQ(IoC::Resolve<bool>("logic_not", false), true);    // from sub_scope1
    });
}
// Root - sub_scope1 - sub_scope1_1
//                     ^
TEST(TestScopeTree_basic_dependencys, IoC_Scope_Current__set_parent_scope)
{
    EXPECT_NO_THROW({
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("Root"))->execute();
    });
    EXPECT_EQ(ScopeTree::getCurScopeId(), "Root");
}
// Root - sub_scope1 - sub_scope1_1
// ^
TEST(TestScopeTree_Threads, threads_in_same_location)
{
    // Root - sub_scope1 - sub_scope1_1
    // ^                 \
    //                     sub_scope1_2
    //                     ^^
    std::thread thr1([](){
        EXPECT_NO_THROW({
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
            IoC::Resolve<ICommand*>("IoC.Scope.New", std::string("sub_scope1_2"))->execute();
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_2"))->execute();
            IoC::Resolve<ICommand*>("IoC.Register",
                                    std::string("logic_or"),
                                    New_Resolver(static_cast<bool(*)(bool,bool)>([](bool b1, bool b2){
                                        return b1 || b2;
                                    })))->execute();
            EXPECT_EQ(IoC::Resolve<bool>("logic_or", false, true), true);   // from sub_scope1_2
        });
    });

    thr1.join();
    std::thread thr2([](){
        EXPECT_NO_THROW({
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_2"))->execute();
            EXPECT_EQ(IoC::Resolve<bool>("logic_or", false, false), false); // from sub_scope1_2
        });
    });
    thr2.join();

}
TEST(TestScopeTree_Threads, threads_in_different_location)
{
    // Root - sub_scope1 - sub_scope1_1
    // ^                 \ ^
    //                     sub_scope1_2
    //                     ^
    std::thread thr1([](){
        EXPECT_NO_THROW({
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_1"))->execute();
            IoC::Resolve<ICommand*>("IoC.Register",
                                    std::string("logic_and"),
                                    New_Resolver(static_cast<bool(*)(bool,bool)>([](bool b1, bool b2){
                                        return b1 && b2;
                                    })))->execute();
            EXPECT_EQ(IoC::Resolve<bool>("logic_and", true, false), false);     // from sub_scope1_1
            EXPECT_EQ(IoC::Resolve<bool>("logic_not", false), true);
        });
    });

    std::thread thr2([](){
        EXPECT_NO_THROW({
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
            IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_2"))->execute();
            EXPECT_EQ(IoC::Resolve<bool>("logic_or", true, true), true);        // from sub_scope1_2
        });
    });
    thr1.join();
    thr2.join();
}
TEST(TestScopeTree_Threads, threads_in_different_location_dont_see_each_other_but_see_common_root)
{
    // Root - sub_scope1 - sub_scope1_1
    // ^                 \ ^
    //                     sub_scope1_2
    //                     ^
    std::thread thr1([](){
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_1"))->execute();
        EXPECT_THROW({
            EXPECT_EQ(IoC::Resolve<bool>("logic_or", true, true), true);    // from sub_scope1_2
        }, IoCException);
        EXPECT_NO_THROW({
            EXPECT_EQ(IoC::Resolve<bool>("logic_not", true), false);        // from sub_scope1
        });
    });

    std::thread thr2([](){
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1"))->execute();
        IoC::Resolve<ICommand*>("IoC.Scope.Current", std::string("sub_scope1_2"))->execute();
        EXPECT_THROW({
            EXPECT_EQ(IoC::Resolve<bool>("logic_and", false, false), false);// from sub_scope1_1
        }, IoCException);
        EXPECT_NO_THROW({
            EXPECT_EQ(IoC::Resolve<bool>("logic_not", false), true);        // from sub_scope1
        });
    });
    thr1.join();
    thr2.join();
}
