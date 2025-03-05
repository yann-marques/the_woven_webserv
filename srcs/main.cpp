#include "WebServ.hpp"

int main(int argc, char **argv, char **envp) 
{
    if (argc >= 2) {
        WebServ ws(argv[1], argv, envp);
    }


    return (0);
}