/*
 * idata_ready.h
 *
 *  Created on: Jan 25, 2016
 *      Author: rafal
 */

#ifndef IDATA_READY_H_
#define IDATA_READY_H_

namespace mr
{
	class idata_ready
	{
	public:
		virtual void on_data_ready()=0; /// process data which arrived to file
		virtual ~idata_ready(){};
	protected:
		using delegate = std::function<void()>;
		delegate _data_ready_handler; /// function object which holds data read event handler function
	};
}

#endif /* IDATA_READY_H_ */
