/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 22:17:26 by lfrederi          #+#    #+#             */
/*   Updated: 2023/06/29 22:17:50 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include "WebServ.hpp"
#include "Debugger.hpp"
#include "Parser.hpp"

extern volatile bool g_run;

static int  printUsage(char const *const prog_name) {
    std::cerr << "Usage: " << prog_name << " [-v] [file.conf]" << std::endl;
    return (1);
}

int     main(int argc, const char **argv) 
{
	WebServ webServ;
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
		parser.parseConfFile(webServ);
        webServ.epollInit();
    }
    catch (const std::exception &e) {
        std::cerr << "Error config: " << e.what() << std::endl;
        return (1);
    }

    webServ.start();

	return (0);
}
