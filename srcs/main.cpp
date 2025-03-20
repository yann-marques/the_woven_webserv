#include "WebServ.hpp"

static void	handleSignal(int signal) {
	if (signal == SIGINT || signal == SIGQUIT)
		throw (WebServ::SignalException(signal));
}


int main(int argc, char **argv, char **envp) 
{
	#ifdef BONUS
		std::cout << "BONUS is defined" << std::endl;
	#endif

    if (argc >= 2) {
        try {
            signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
            WebServ ws(argv[1], argv, envp);

            signal(SIGINT, handleSignal);
            signal(SIGQUIT, handleSignal);
            ws.listenEvents();
        }
        // AParser exceptions
	    catch (AParser::ArgOutOfServerScopeException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (AParser::ConfigSyntaxException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (AParser::UnexpectedKeyException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (AParser::UnexpectedValueException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (AParser::DoubleArgException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (AParser::ForbiddenCharException& e) {
	    	std::cerr << e.what() << std::endl;
	    }
        // Config exceptions
	    catch (Config::IsDirException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (Config::OpenFileException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (Config::UnclosedScopeException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (Config::BadSpacesException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (Config::MissingPortException& e) {
	    	std::cerr << e.what() << std::endl;
	    }
        // Rules exceptions
	    catch (Rules::RedefinedArgException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (Rules::InvalidLocationKeyException& e) {
	    	std::cerr << e.what() << std::endl;
	    }
        // WebServ construction exceptions
	    catch (WebServ::SignalException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCreateException& e) {
	    		std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollWaitException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCtlAddException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCtlDelException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::UnknownFdException& e) {
	    	std::cerr << e.what() << std::endl;
	    }
        // VServ construction exceptions
	    catch (VServ::SocketException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (VServ::SetSockOptException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (VServ::BindException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (std::exception& e) {}
    }
    return (0);
}