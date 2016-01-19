
/*
 * robot_controller.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: rafal
 */
#include "robot_controller.h"

namespace mrobot
{
robot_controller::robot_controller(): robot_controller("/dev/ttyS98")
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
	try
	{
		// initialize serial device with default parameters
		_serial_device = new serial_port(serial_device);
		_serial_device->subscribe_data_ready_event(std::bind(&robot_controller::serial_event_handler,this));

		_poll_controller.add(_serial_device);

		// turn on camera app if path is given
		if(!camera_script_path.empty())
		{
			_camera_app.run_script(camera_script_path);
		}

		// initialize server on default port
		_server = new tcp_server();
		_server->subscribe_data_ready_event(std::bind(&robot_controller::server_event_handler, this));

		_poll_controller.add(_server);

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
			if(_serial_data_ready.exchange(false))
			{
				_serial_device->receive_data(_serial_buffer);
//
				//std::cout<<"Data from serial device ("<<_serial_buffer.size()<<")\n";
				//for(char&c : _serial_buffer)
				//	std::cout<<c;
				//std::cout<<"\n";//

				_server->send_data(_serial_buffer);
			}

			if(_server_data_ready.exchange(false))
			{
				_server->receive_data(_server_buffer);
//
				//std::cout<<"Data from server device ("<<_server_buffer.size()<<")\n";
				//for(char&c : _server_buffer)
				//	std::cout<<c;
				//std::cout<<"\n";//

				_serial_device->send_data(_server_buffer);
			}
		}
		_poll_controller.stop_polling();

}

/**
 * @brief Handles server data ready event
 * In this version only sends command to microcontroller
 * @param server
 * @param buffer
 */
void robot_controller::server_event_handler()
{
	_server_data_ready.store(true);
}

void robot_controller::serial_event_handler()
{
	_serial_data_ready.store(true);
}

/**
 * @brief Starts application which will send video through wi-fi
 */
void robot_controller::start_camera_app(std::string _camera_script_path)
{
	_camera_app.run_script(_camera_script_path);
}

} /* mrobot namespace */

