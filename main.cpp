#include <iostream>
#include "SpaceBattle/IoCContainer/IoCCore/IoC.h"
#include <functional>
#include <map>
#include <string>
#include "SpaceBattle/ICommand.h"
#include "SpaceBattle/IoCContainer/ScopeTree.h"
#include "SpaceBattle/IoCContainer/IoCCore/IIoCContainer.h"
#include "SpaceBattle/IoCContainer/IoCConnectContainerCommand.h"

#include <thread>

using namespace std;

static void thread1Runner(void)
{
    cout << "Hello thread1!" << endl;

    IoC::Resolve<ICommand*>("IoC.Scope.New", string("scope_thr1"))->execute();
    IoC::Resolve<ICommand*>("IoC.Scope.Current", string("scope_thr1"))->execute();

    static std::function<float(float,int)> fun1 = [](float i1, int i2) -> float {
        return i1+i2;
        };

    IoC::Resolve<ICommand*>(
                "IoC.Register",
                string("test1"),
                New_Resolver(fun1))->execute();


    IoC::Resolve<ICommand*>(
                "IoC.Register",
                string("test3"),
                New_Resolver((std::function<void(void)>)[&]() -> void {
                     cout << "hello IoC! " << fun1(54.5f, 76) << endl;
                }))->execute();

    cout << IoC::Resolve<float>("test1", 3.243f, 23) << endl;
    //cout << IoC::Resolve<bool>("test2") << endl;
    IoC::Resolve<ICommand*>("IoC.Scope.Delete")->execute();
}

static void thread2Runner(void)
{
    cout << "Hello thread2!" << endl;


    IoC::Resolve<ICommand*>("IoC.Scope.New", string("scope_thr2"))->execute();

    IoC::Resolve<ICommand*>("IoC.Scope.Current", string("scope_thr2"))->execute();
    //IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("scope_thr1"))->execute();



    IoC::Resolve<ICommand*>(
                "IoC.Register",
                string("test2"),
                New_Resolver(static_cast<bool(*)()>([]() -> bool{
                    return true;
                })))->execute();

    //cout << IoC::Resolve<float>("test1", 3.243f, 23) << endl;
    cout << IoC::Resolve<bool>("test2") << endl;
}

int main()
{


    IoCConnectContainerCommand((IIoCContainer*)&ScopeTree::g_scopeTree).execute();

    ICommand* cmd;
    cmd = IoC::Resolve<ICommand*>("IoC.Scope.New", string("scope_main"));
    cmd->execute();
    delete cmd;

    std::thread thr1(&thread1Runner);
    thr1.join();
    std::thread thr2(&thread2Runner);
    thr2.join();

    //IoC::Resolve<ICommand*>(string("IoC.Scope.Delete"))->execute();


    cout << "Hello main!" << endl << endl << endl;

    //IoC::Resolve<void>("test3");

    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("scope_main"))->execute();

    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("1"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("2"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("3"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("2"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Delete"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("scope_main"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("4"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("4"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("41"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("42"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("43"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("44"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("scope_main"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("3"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("31"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("31"))->execute();

    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("311"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("312"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("311"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("3111"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("3111"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.New"), string("31111"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("311"))->execute();
    //IoC::Resolve<ICommand*>(string("IoC.Scope.Delete"))->execute();
    //IoC::Resolve<ICommand*>(string("IoC.Scope.Delete"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("31"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("3"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Current"), string("scope_main"))->execute();
    IoC::Resolve<ICommand*>(string("IoC.Scope.Delete"))->execute();

    return 0;
}
