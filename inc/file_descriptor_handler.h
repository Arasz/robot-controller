/*
 * file_descriptor_handler.h
 *
 *  Created on: Jan 16, 2016
 *      Author: rafal
 */

#ifndef INC_FILE_DESCRIPTOR_HANDLER_H_
#define INC_FILE_DESCRIPTOR_HANDLER_H_

#include <mutex>
#include <unistd.h>

namespace mr
{
	class file_descriptor_handler
	{
		using mutex = std::mutex;

	public:
		file_descriptor_handler(): file_descriptor_handler(-3){}
		file_descriptor_handler(int file_descriptor):_file_descriptor(file_descriptor){}
		file_descriptor_handler(const file_descriptor_handler& file_descriptor) = delete;
		virtual ~file_descriptor_handler(){close_file_descriptor();}

		file_descriptor_handler& operator=(int file_descriptor)
		{
			using std::swap;
			swap(_file_descriptor, file_descriptor);
			return *this;
		}

		mutex& get_mutex() {return _fd_mutex;}
		int get_file_descriptor() {return _file_descriptor;}
		void close_file_descriptor(){ close(_file_descriptor); }

	private:
		int _file_descriptor;
		mutex _fd_mutex;
	};
} /* namespace mrobot */

#endif /* INC_FILE_DESCRIPTOR_HANDLER_H_ */
