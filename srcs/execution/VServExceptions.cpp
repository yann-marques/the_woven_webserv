#include "VServ.hpp"

const char*	VServ::SocketException::what() const throw() {
	return ("Failed to create socket.");
}

const char*	VServ::SetSockOptException::what() const throw() {
	return ("Failed to set socket opt.");
}

const char*	VServ::BindException::what() const throw() {
	return ("Failed to bind socket.");
}

const char*	VServ::ListenException::what() const throw() {
	return ("Failed to listen.");
}

const char*	VServ::AcceptException::what() const throw() {
	return ("Failed accept connection on socket.");
}

const char*	VServ::RecvException::what() const throw() {
	return ("Failed to read the request in the buffer.");
}

const char*	VServ::SendException::what() const throw() {
	return ("Failed to send the request to the clientfd");
}

const char*	VServ::SendPartiallyException::what() const throw() {
	return ("Failed to send entire request to the client");
}

const char*	VServ::ReadFileException::what() const throw() {
	return ("Fail read the file");
}

const char*	VServ::FileNotExist::what() const throw() {
	return ("Fail to get infos about the file. File not exist.");
}

const char*	VServ::OpenFileException::what() const throw() {
	return ("Error to opening the root file");
}

const char*	VServ::OpenFolderException::what() const throw() {
	return ("Error to opening the folder");
}

const char*	VServ::EntityTooLarge::what() const throw() {
	return ("Error, the entity is too lage. Change client_max_body_size in config");
}

const char*	VServ::ExtensionNotFound::what() const throw() {
	return ("Error, extension for the cgi is not found on request path");
}

const char*	VServ::PipeException::what() const throw() {
	return ("Error, the pipe function make an excetion");
}

const char*	VServ::ForkException::what() const throw() {
	return ("Error, the fork function make an exception");
}

const char* VServ::ExecveException::what() const throw() {
	return ("Execve error. Can't execute the binary cgi");
}

const char* VServ::ChildProcessException::what() const throw() {
	return ("ChildProcessException, the process has failed. Maybe CGI script not found.");
}

const char* VServ::ServerNameNotFound::what() const throw() {
	return ("Server name not found in config. Abort.");
}

const char* VServ::InterpreterEmpty::what() const throw() {
	return ("Interpreter is empty.");
}

const char* VServ::MethodNotAllowed::what() const throw() {
	return ("The method is not allowed. Abort");
}

const char* VServ::CreateFileException::what() const throw() {
	return ("Error, can't create file exception");
}

const char* VServ::NoUploadFileName::what() const throw() {
	return ("User need to pass a filename to upload content");
}

const char* VServ::EpollCreateException::what() const throw() {
	return ("EpollCreate exception");
}


const char* VServ::EpollCTLException::what() const throw() {
	return ("EpollCTL Exception");
}

const char* VServ::EpollWaitException::what() const throw() {
	return ("Epoll wait exeception");
}