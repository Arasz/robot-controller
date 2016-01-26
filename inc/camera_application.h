/*
 * camera_app_controller.h
 *
 *  Created on: Jan 14, 2016
 *      Author: rafal
 */

#ifndef INC_CAMERA_APPLICATION_H_
#define INC_CAMERA_APPLICATION_H_

#include <sys/unistd.h>
#include <stdexcept>
#include <string>
#include <errno.h>
#include <cstring>
#include <sys/prctl.h>
#include <sys/signal.h>

namespace mr
{
	/**
	 * @brief Runs application with is responsible for camera control and streaming
	 */
	class camera_application
	{
	public:
		void run_script(std::string script_path);
	};
} /*namespace mrobot*/
#endif /* INC_CAMERA_APPLICATION_H_ */
