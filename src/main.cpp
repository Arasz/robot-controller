/*
 * main.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: rafal
 */

#include "robot_controller.h"
#include <string>


int main(int argc, char* argv[])
{
	using namespace mr;

	std::unique_ptr<robot_controller> controler;

	if(argc > 1) // first argument - program name
	{
		// there are command line arguments
		std::string serial_device = "/dev/ttyAMA0";
		std::string script_path = "";
		int port = 22222;

		for(int i = 1; i < argc; i++)
		{
			if(std::string{argv[i]} == "-h")
			{
				std::cout<<"Usage:\n"
						<<"-t path - path to tty device (serial device)\n"
						<<"-s path - path to camera script file\n"
						<<"-p port - port on which server will listen\n"
						<<"example: robot_controller -t /dev/ttyS98 -s /home/usr/Desktop/camera_script.sh -p 22222"
						<<std::endl;
				std::exit(0);
			}
			if(std::string{argv[i]} == "-t")
			{
				serial_device = std::string{argv[++i]};
			}
			if(std::string{argv[i]} == "-s")
			{
				script_path = std::string{argv[++i]};
			}
			if(std::string{argv[i]} == "-p")
			{
				try
				{
					port = std::stoi(std::string{argv[++i]});
				}
				catch(std::invalid_argument& ex)
				{
					std::cerr<<ex.what()<<"\n"<<"Will use port number: "<<port<<"\n";
				}
				catch(std::out_of_range& ex)
				{
					std::cerr<<ex.what()<<"\n"<<"Will use port number: "<<port<<"\n";
				}
			}
		}
		controler = std::move(std::unique_ptr<robot_controller>(new robot_controller{serial_device, script_path, port}));
	}
	else
	{
		std::cout<<"Application will be started with default configuration. Try robot_controller -h for usage example.\n";
		controler = std::move(std::unique_ptr<robot_controller>{new robot_controller()});
	}

	while(true)
	{
		controler->start_controler();
	}

	return 0;
}
