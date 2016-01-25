/*
 * poll_controler.cpp/
 *
 *  Created on: Jan 12, 2016
 *      Author: rafal
 */

#include "poll_controler.h"


namespace mr
{

poll_controler::poll_controler() :
		poll_controler(-1, milliseconds(500))
{

}

poll_controler::poll_controler(int poll_timeout,
		milliseconds poll_interval) :
		_timeout(poll_timeout), _poll_interval(poll_interval)
{

}

poll_controler::~poll_controler()
{
	delete[] _ufds;
	_is_poll_thread_running = false;
	_poll_thread.join();
}
/**
 * @brief Adds object to polling list
 * @param observer object which has file descriptor and want to know when data is ready to read
 */
void poll_controler::add(ifile_descriptor_owner* observer)
{
	_observers.push_back(observer);
}
/**
 * @brief Removes object from polling list
 * @param observer object which has file descriptor and want to know when data is ready to readr
 */
void poll_controler::remove(ifile_descriptor_owner* observer)
{
	_observers.erase(std::remove(_observers.begin(), _observers.end(), observer));
}
/**
 * @brief Starts polling in another thread
 */
void poll_controler::start_polling()
{
	construct_ufds_array();
	_poll_thread = std::thread{ &poll_controler::poll_loop, this };
	_is_poll_thread_running = true;;
	//std::cerr<<">> "<<"Thread started\n";
}
/**
 * @brief Stops polling thread
 */
void poll_controler::stop_polling()
{
	_is_poll_thread_running = false;
	_poll_thread.join();
}

/**
 * @brief Main poll loop.
 * Sleeps for _poll_interval and then polls file descriptors
 */
void poll_controler::poll_loop()
{
	while (_is_poll_thread_running)
	{
		std::this_thread::sleep_for(_poll_interval);
		try
		{
			poll_file_descriptors();
		} catch (poll_exception& ex)
		{
			std::cerr << ex.what();
		}
	}
}

/**
 * @brief Polls file descriptors for data ready to read
 * @throws poll_exception
 */
void mr::poll_controler::poll_file_descriptors()
{
	if (_ufds_size == _observers.size())
	{
		// events_count equal to zero means timeout
		int events_count = poll(_ufds, _ufds_size, _timeout);

		if (events_count < 0)
			throw poll_exception
			{ "Error when polling file descriptors.", strerror(errno) };
		else
		{
			for (unsigned int i = 0; (i < _ufds_size) && (events_count > 0); i++)
			{
				if (_ufds[i].revents & POLLIN)
				{
					// by construction element _ufds[i] has this same fd as _observers[i]
					idata_ready* casted_pointer = dynamic_cast<idata_ready*>(_observers[i]); // Cast with dynamic cast ( runtime check)
					if(casted_pointer != nullptr)
						casted_pointer->on_data_ready();
					else
						throw  poll_exception{"Wrong casting"};
					events_count--;
				}
			}
		}
	}
	else
	{
		construct_ufds_array();
	}
}
/**
 * @brief Constructs array off file descriptor structures used for polling. This method blocks when file descriptor is not ready.
 */
void mr::poll_controler::construct_ufds_array()
{

	_ufds_size = _observers.size();

	if (_ufds != nullptr)
		delete[] _ufds;

	_ufds = new pollfd[_ufds_size];

	int i = 0;
	for (auto observer : _observers)
	{
		while(!observer->is_file_descriptor_ready());
		_ufds[i].fd = observer->get_file_descriptor_handler().get_file_descriptor();
		_ufds[i++].events = POLLIN; // return from poll() when data is ready to read on fd
	}
}

}
