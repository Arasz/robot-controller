
/*
 * robot_controller.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: rafal
 */
#include "robot_controller.h"

namespace mrobot
{
robot_controller::robot_controller(): robot_controller("/dev/AM0", 22222)
{
}

robot_controller::robot_controller(std::string serial_device, int port)
{
	using namespace std::placeholders;
	try
	{
		// initialize serial device with default parameters
		_serial_device = new serial_port(serial_device);
		std::function<void(serial_port&, std::vector<char>&)> handler = std::bind(&robot_controller::serial_event_handler,this, _1, _2);
		_serial_data_ready_event_handler = handler;
		_serial_device->subscribe_data_ready_event(_serial_data_ready_event_handler);
		// initialize server on default port
		_server = new tcp_server();
		_server_data_ready_event_handler = std::bind(&robot_controller::server_event_handler, this, _1, _2);
		_server->subscribe_data_ready_event(_server_data_ready_event_handler);
		// turn on camera app

	}
	catch(serial_port_exception& ex)
	{

	}
	catch(tcp_server_exception& ex)
	{

	}
}

robot_controller::~robot_controller()
{
	delete _serial_device;
	delete _server;
}

void robot_controller::start_controler()
{
}


void robot_controller::server_event_handler(tcp_server& server,
		std::vector<char>& buffer)
{
}

void robot_controller::serial_event_handler(serial_port& server,
		std::vector<char>& buffer)
{
}

} /* mrobot namespace */

