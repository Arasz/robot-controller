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
	/**
 	 * @brief Interface from which will derivative all classes implementing data_ready event
 	 */
	class idata_ready
	{
	public:
		virtual void on_data_ready()=0; /// triggers event handler when data is ready to read
		virtual ~idata_ready(){};
	protected:
		using delegate = std::function<void()>;
		delegate _data_ready_handler; /// function object which holds data read event handler
	};
}

#endif /* IDATA_READY_H_ */
