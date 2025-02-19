#include "WebServ.hpp"

int main(int argc, char **argv, char **envp) 
{
    (void) argc;

    WebServ ws("filename", argv, envp);

    std::cout << ws;
    return (0);
}