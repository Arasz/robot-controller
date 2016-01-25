/*
 * serial_port.h
 *
 *  Created on: Dec 13, 2015
 *      Author: rafal
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include <termios.h> // Library used for tty devices configuration

#include <fcntl.h> // File control library
#include <sys/types.h> // System data types
#include <sys/stat.h> // Defines the structure of the data returned by the function stat() ( file status)
#include <sys/unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <exception>
#include <sys/ioctl.h>
#include <vector>
#include "serial_port_exception.h"
#include "serial_port_util.h"
#include <errno.h>
#include <functional>
#include "ifile_descriptor_owner.h"
#include "file_descriptor_handler.h"
#include <mutex>

namespace mrobot
{
	/**
	 * @brief Class with allows easy serial port communication in linux.
	 */
	class serial_port: public ifile_descriptor_owner
	{
		//using serial_delegate = std::function<void()>;

	public:

		serial_port(std::string device, baudrate_option baudrate = baudrate_option::b9600, data_bits_option data_bits = data_bits_option::eight,
				parity_option parity = parity_option::none, stop_bits_option stop_bits=stop_bits_option::one);
		virtual ~serial_port();
		serial_port& operator =(serial_port&)=default;

		void open_device(std::string device);
		void configure(baudrate_option baudrate, data_bits_option data_bits,
				parity_option parity, stop_bits_option stop_bits);

		void send_data(const std::vector<char>& buffer);
		int is_data_ready();
		void receive_data(std::vector<char>& buffer);

		void subscribe_data_ready_event(delegate&& event_handler);
		void unsubscribe_data_ready_event();

		bool is_ready(){ return _is_opend&&_is_configured;}

		virtual void process_data() override;
		virtual file_descriptor_handler& get_file_descriptor_handler() override;
		virtual bool is_file_descriptor_ready() override;

	private:

		void read_data();
		const int _buffer_size = 255; /// data buffer size
		char _system_interaction_buffer[255]{}; /// data buffer which will be passed to system functions
		std::vector<char> _received_data_buffer; /// data buffer whith received data
		std::mutex _buffer_mutex;


		bool _is_data_ready_event_subscribed = false; /// indicates that data ready event is subscribed
		delegate _data_ready_event_handler; /// function called when data ready event occurs

		bool _is_opend = false;
		bool _is_configured = false;

		const std::string _device; /// path to device
		file_descriptor_handler _serial_device; /// device file descriptor
	};
} /* namespace mrobot */

#endif /* SERIALPORT_H_ */
