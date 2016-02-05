/*
 * iobserver.h
 *
 *  Created on: Jan 12, 2016
 *      Author: rafal
 */

#ifndef INC_IFILE_DESCRIPTOR_OWNER_H_
#define INC_IFILE_DESCRIPTOR_OWNER_H_

#include "file_descriptor_handler.h"

namespace mr
{
	/**
	 * @brief Interface from which will derivative all classes which can be updated and have file descriptor
	 */
	class ifile_descriptor_owner
	{

	public:
		virtual file_descriptor_handler& get_file_descriptor_handler()=0; /// gets file descriptor
		//FIXME Move is_file_descriptor_ready() to the file_descriptor_handler_class
		//virtual bool is_file_descriptor_ready()=0; /// checks if file descriptor is acquired by class
		virtual ~ifile_descriptor_owner() {};
	};
}



#endif /* INC_IFILE_DESCRIPTOR_OWNER_H_ */
