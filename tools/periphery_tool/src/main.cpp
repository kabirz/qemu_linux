#include <Command.h>
#include <cli/cli.h>
#include <cli/loopscheduler.h>
DeviceRegistry DeviceRegistry::instance;

int main()
{
    try {
        // setup periphery_tool
        auto rootMenu = make_unique<Menu>("periphery_tool", "periphery interactive shell");
        DeviceRegistry::Instance().SetMenu(*rootMenu);
	DeviceRegistry::Instance().LoadAll();

        SetColor();
        Cli cli( std::move(rootMenu) );
        // global exit action
        cli.ExitAction( [](auto& out){ out << "Exit periphery tool.\n"; } );

        cli::LoopScheduler scheduler;
        CliLocalTerminalSession localSession(cli, scheduler, std::cout, 200);
        localSession.ExitAction(
            [&scheduler](auto& out) // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            }
        );

        scheduler.Run();

        return 0;
    } catch (const std::exception& e) {
        cerr << "Exception caugth in main: " << e.what() << '\n';
    } catch (...) {
        cerr << "Unknown exception caugth in main.\n";
    }
    return -1;
}
