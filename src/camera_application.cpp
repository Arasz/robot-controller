/*
 * camera_app_controller.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: rafal
 */

#include "camera_application.h"

using namespace std;

namespace mrobot
{

camera_application::camera_application()
{

}

camera_application::~camera_application()
{
}

void camera_application::run_script(string script_path)
{
	// get parent process id
	// pid_t parent_pid = getpid();

	// fork this process
	pid_t pid = fork();
	if(pid<0)
	{
		throw runtime_error{"Error, filed to fork()"};
	}
	else if(pid>0)
	{
		// pid is greater than zero, this code will be executed inside parent process
	}
	else
	{
		// pid is equal zero - this code will be executed inside child process

		prctl(PR_SET_PDEATHSIG, SIGHUP); // ask kernel to send SIGHTUP signal when parent dies (close child process)

		// run camera script inside this process

		// script starts
		int result = execl(script_path.c_str(), "camera_script", (char*)NULL);

		// on succes execv dose not return

		if(result < 0)
		{
			string message = "Error when executing script: " + string{strerror(errno)} +"\n";
			throw runtime_error{message};
		}

	}
}

} /*namespace mrobot*/


