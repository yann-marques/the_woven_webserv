/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:35 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:38 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

static void	handleSignal(int signal) {
	if (signal == SIGINT || signal == SIGQUIT)
		throw (WebServ::SignalException(signal));
}


int main(int argc, char **argv, char **envp) 
{
    if (argc >= 2) {
        try {
            signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
			signal(SIGPIPE, SIG_IGN);
			WebServ ws(argv[1], argv, envp);

            signal(SIGINT, handleSignal);
            signal(SIGQUIT, handleSignal);
            ws.listenEvents();
        }
        // WebServ construction exceptions
	    catch (WebServ::SignalException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCreateException& e) {
	    		std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCtlAddException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::EpollCtlDelException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (WebServ::UnknownFdException& e) {
	    	std::cerr << e.what() << std::endl;
	    } catch (std::exception& e) {}
    } else {
		std::cerr << "Webserv error: you need to pass a config file." << std::endl << "Usage: ./webserver <config_filename>" << std::endl;
	}
    return (0);
}