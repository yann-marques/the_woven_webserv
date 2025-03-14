#include "WebServ.hpp"

static void	handleSignal(int signal) {
	if (signal == SIGINT)
		throw (WebServ::SIGINTException());
}


int main(int argc, char **argv, char **envp) 
{
    if (argc >= 2) {
        try {
            signal(SIGINT, SIG_IGN);
            WebServ ws(argv[1], argv, envp);

            signal(SIGINT, handleSignal);
            ws.listenEvents();
        } catch (WebServ::SIGINTException& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return (0);
}