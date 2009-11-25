// deliciousServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tcpserver.h"
#include "naked_conn.h"
#include "deliciousDataAdapter.h"
#include <string>
#include <exception>
#include <iostream>
#include <pantheios/backends/bec.file.h>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::program_options;

extern "C"
const char PANTHEIOS_FE_PROCESS_IDENTITY[]  =   "deliciousServer";

static const int MonitorPort = 24000;

boost::asio::io_service io;
rclib::network::TCPServer<naked_conn> *serverd;

bool InitializeComponents(const string& dbpath);

#ifndef WIN32
int main(int argc, char* argv[])
#else
int _tmain(int argc, TCHAR* argv[])
#endif
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

	options_description desc("Allowed options for delicious server:");
	desc.add_options()
		("help", "Output this message.")
		("db", value<string>()->default_value("delicacyDB.db3"), "Opens sqlite database at given location.")
		("logpath", value<string>()->default_value("delicious.log"), "Write run log to given path.");

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);    

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

    //////////////////////////////////////////////////////////////////////////
    pantheios_be_file_setFilePath(vm["logpath"].as<string>().c_str());
    //////////////////////////////////////////////////////////////////////////

    pantheios::log_NOTICE("Server launched.");
    
    if (!InitializeComponents(vm["db"].as<string>()))
    {
        pantheios::log_NOTICE("Initialize Faile. Terminating...");
        return -1;
    }
    serverd->start();    
    pantheios::log_NOTICE("Server Start.");
    boost::array<boost::thread, 4> threadpool;
    BOOST_FOREACH(boost::thread& t, threadpool)
        t = boost::thread(boost::bind(&io_service::run, &io));

    io.run();

    BOOST_FOREACH(boost::thread& t, threadpool)
        t.join();
    pantheios::log_NOTICE("Server Stoped normally.");
    return 0;
}

bool InitializeComponents(const string& dbpath)
{
    pantheios::log_NOTICE("Begin Initialize.");
    try
    {
        serverd = new rclib::network::TCPServer<naked_conn>(io, MonitorPort);
        pantheios::log_INFORMATIONAL("Network daemon initialized.");
        deliciousDataAdapter::Initialize(dbpath);
        pantheios::log_INFORMATIONAL("Database initialized.");

        pantheios::log_NOTICE("Initialization Finished Successfully.");
        return true;
    }
    catch (exception& e)
    {
        pantheios::log_CRITICAL(e.what());

        return false;
    }
}
