#include "Server.hpp"

int main(void) {

    //Idee: si il y a plusieurs server virtuel dans la config: une for loop avec new Server(int: port) a chaque iteration ? plus simple et plus modulaire. Un server, une instance.
    //En l'orrurence ici un seul sur le port 8080
    Server server = Server(8080);

    server.start();
    return (0);
}