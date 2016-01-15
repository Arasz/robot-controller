
/*
 * robot_controller.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: rafal
 */
#include "robot_controller.h"

namespace mrobot
{
robot_controller::robot_controller(): robot_controller("/dev/AM0")
{
}

/**
 * @brief Create new serial_device and server, subscribe events, add objects to poll controller and run camera script if path was given
 * @param serial_device serial device (tty) file path
 * @param camera_script_path absolute path to camera script
 * @param port port number on which server will listen
 */
robot_controller::robot_controller(std::string serial_device, std::string camera_script_path, int port)
{
	using namespace std::placeholders;
	try
	{
		// initialize serial device with default parameters
		_serial_device = new serial_port(serial_device);
		_serial_data_ready_event_handler = std::bind(&robot_controller::serial_event_handler,this, _1, _2); // retval: function<void(serial_port&, std::vector<char>&)>
		_serial_device->subscribe_data_ready_event(_serial_data_ready_event_handler);

		if(_serial_device->is_ready())
			_poll_controller.add(_serial_device);

		// initialize server on default port
		_server = new tcp_server();
		_server_data_ready_event_handler = std::bind(&robot_controller::server_event_handler, this, _1, _2);
		_server->subscribe_data_ready_event(_server_data_ready_event_handler);

		if(_server->is_connected())
			_poll_controller.add(_server);

		// turn on camera app if path is given
		if(!camera_script_path.empty())
		{
			_camera_app.run_script(camera_script_path);
		}

	}
	catch(serial_port_exception& ex)
	{
		std::cerr<<ex.what();
		throw ex;
	}
	catch(tcp_server_exception& ex)
	{
		std::cerr<<ex.what();
		throw ex;
	}
}

robot_controller::~robot_controller()
{
	delete _serial_device;
	delete _server;
}

void robot_controller::start_controler()
{
		_poll_controller.start_polling();

		while(_server->is_connected())
		{

		}

		_poll_controller.stop_polling();
		_server->reconnect();
}

/**
 * @brief Handles server data ready event
 * In this version only sends command to microcontroller
 * @param server
 * @param buffer
 */
void robot_controller::server_event_handler(tcp_server& server,
		std::vector<char>& buffer)
{
	if(buffer.size()<=_tcp_buffer_size)
	{
		std::copy(buffer.begin(), buffer.end(),_tcp_buffer.begin());
	}
	_serial_device->send_data(_tcp_buffer);
}

void robot_controller::serial_event_handler(serial_port& server,
		std::vector<char>& buffer)
{
	if(buffer.size()<=_serial_buffer_size)
	{
		std::copy(buffer.begin(), buffer.end(),_serial_buffer.begin());
	}
	_server->send_data(_serial_buffer);
}

/**
 * @brief Starts application which will send video through wi-fi
 */
void robot_controller::start_camera_app(std::string _camera_script_path)
{
	_camera_app.run_script(_camera_script_path);
}

} /* mrobot namespace */

