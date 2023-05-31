
#include <iostream>
#include <cstring>
#include "Server.hpp"
#include "Debugger.hpp"
#include "Parser.hpp"

extern volatile bool g_run;
int									g_epollFd; // REMOVE


static int  printUsage(char const *const prog_name) {
    std::cerr << "Usage: " << prog_name << " [-v] [file.conf]" << std::endl;
    return (1);
}

int     main(int argc, const char **argv) 
{
	std::map<int, Server> mapServers;

	argc = 1;
    if (argv[argc] && !std::strcmp(argv[argc], "-v"))
	{
        DEBUG_START(true);
		argc++;
	}

    if (argv[argc] == NULL)
        argv[argc] = "conf/default.conf";
    else if (argv[argc + 1] != NULL)
        return (printUsage(argv[0]));

    try
	{
        Parser parser(argv[argc]);
		parser.parseConfFile(mapServers);
		//autoindex autind("/");
		//std::cout<<autind.getIndexPage()<<std::endl;
		//std::cout << serverlist << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Error config: " << e.what() << std::endl;
        return (1);
    }


	return (0);
}
