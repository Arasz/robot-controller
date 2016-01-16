/*
 * iobserver.h
 *
 *  Created on: Jan 12, 2016
 *      Author: rafal
 */

#ifndef INC_IFILE_DESCRIPTOR_OWNER_H_
#define INC_IFILE_DESCRIPTOR_OWNER_H_

#include "file_descriptor_handler.h"

namespace mrobot
{
	/**
	 * @brief Interface from which will derivative all class which can be updated and have fille descriptor
	 */
	class ifile_descriptor_owner
	{
	public:
		virtual void process_data()=0; /// process data which arrived to file
		virtual file_descriptor_handler& get_file_descriptor_handler()=0; /// gets file descriptor
		virtual bool is_file_descriptor_ready()=0; /// checks if file descriptor is acquired by class
		virtual ~ifile_descriptor_owner() {};
	};
}



#endif /* INC_IFILE_DESCRIPTOR_OWNER_H_ */
