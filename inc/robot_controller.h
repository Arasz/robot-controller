/*
 * robot_controller.h
 *
 *  Created on: Jan 11, 2016
 *      Author: rafal
 */

#ifndef INC_ROBOT_CONTROLLER_H_
#define INC_ROBOT_CONTROLLER_H_

#include <poll_controller.h>
#include <vector>
#include "tcpserver.h"
#include "serial_port.h"
#include "camera_application.h"
#include <exception>
#include <iterator>
#include <atomic>

namespace mr
{
	class robot_controller
	{
	public:
		robot_controller();
		robot_controller(const robot_controller& ) =delete;// copy constructor deleted
		robot_controller(std::string serial_device, std::string camera_script_path="", int port = 22222);
		virtual ~robot_controller();

		void start_controller();

	private:
		poll_controller _poll_controller; /// polls used file descriptors for data to read

		tcp_server* _server;
		unsigned int _tcp_buffer_size = 255; /// size of tcp buffer
		std::vector<char> _server_buffer{static_cast<char>(_tcp_buffer_size), 0}; /// buffer for data from server
		void server_event_handler();
		std::atomic<bool> _server_data_ready{false};

		serial_port* _serial_device;
		unsigned int _serial_buffer_size = 255;
		std::vector<char> _serial_buffer{static_cast<char>(_serial_buffer_size), 0};
		void serial_event_handler();
		std::atomic<bool> _serial_data_ready{false};

		camera_application _camera_app;
		void start_camera_app(std::string _camera_script_path);

//		uint64_t _serial_to_server = 0;
//		uint64_t _server_to_serial = 0;
	};
}



#endif /* INC_ROBOT_CONTROLLER_H_ */
