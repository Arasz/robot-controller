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
	/**
	 * @brief Handler for file descriptors.
	 * Is responsible for closing files, has mutex object connected with file
	 * descriptor for easy multithread operations on files.
	 */
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

		/**
		 * @brief Gets mutex binded with file descriptor
		 * @return file descriptor mutex
		 */
		mutex& get_mutex() {return _fd_mutex;}
		/**
		 * @brief Gets file descriptor
		 * @return file descriptor
		 */
		int get_file_descriptor() {return _file_descriptor;}

		/**
		 * @brief Checks if file descriptor is acquired and can be used
		 * @return true if file descriptor is ready to use
		 */
		bool is_file_descriptor_ready() { return (_file_descriptor>=0)?true:false; }
		/**
		 * @brief Closes file descriptor
		 */
		void close_file_descriptor(){ close(_file_descriptor); }

	private:
		int _file_descriptor;
		mutex _fd_mutex;
	};
} /* namespace mrobot */

#endif /* INC_FILE_DESCRIPTOR_HANDLER_H_ */
