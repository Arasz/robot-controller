/*
 * descriptors_poller.h
 *
 *  Created on: Jan 12, 2016
 *      Author: rafal
 */

#ifndef INC_POLL_CONTROLER_H_
#define INC_POLL_CONTROLER_H_

#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <sys/poll.h>
#include <cstring>
#include <map>
#include <vector>
#include <exception>
#include <errno.h>
#include <ifile_descriptor_owner.h>
#include <iostream>

namespace mrobot
{
using milliseconds = std::chrono::milliseconds;

/**
 * @brief Stores list of file descriptor owners and informs them when data is ready to read
 */
class poll_controler
{
public:
	poll_controler();
	poll_controler(int poll_timeout, milliseconds poll_interval);
	virtual ~poll_controler();

	void add(ifile_descriptor_owner* observer);
	void remove(ifile_descriptor_owner* observer);

	void start_polling();
	void stop_polling();

private:

	void poll_loop();
	void poll_file_descriptors();
	void construct_ufds_array();

	std::vector<ifile_descriptor_owner*> _observers{};

	std::thread _poll_thread{};

	bool _is_poll_thread_running{false};

	unsigned int _ufds_size{}; /// amount of sockets which are polled by poll()

	int _timeout{-1}; /// time in milliseconds after which poll function() terminates (if negative function never terminates)

	milliseconds _poll_interval{ 500 }; /// time after which polling thread will poll file descriptors

	pollfd* _ufds{nullptr}; /// array of structures representing file descriptors used in file descriptors polling
};



class poll_exception: std::exception
{
public:
	poll_exception(std::string message, std::string error = "None") :
			_message(message), _error_description(error)
	{
	}
	const char* what() const throw () override
	{
		std::string result = ("Message: " + _message + "\n"
				+ "Error description: " + _error_description + "\n");
		return result.c_str();
	}
private:
	const std::string _message{};
	const std::string _error_description{};
};

}

#endif /* INC_POLL_CONTROLER_H_ */
