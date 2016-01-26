/*
 * tcpserver.cpp
 *
 *  Created on: Dec 29, 2015
 *      Author: rafal
 */

#include "tcpserver.h"

namespace mr
{
/**
 * @brief Constructs server object listening on default port
 */
tcp_server::tcp_server(): tcp_server(22222)
{

}

/**
 * @brief Constructs server object listening on given port
 * @param port port on which server is listening for connection
 */
tcp_server::tcp_server(int port):_port(port)
{
	this->create_socket();
	this->bind_socket(_port);
	this->listen_for_connection();

	this->reconnect();
}

tcp_server::~tcp_server()
{
	// fd will be closed when on fd_handler destruction
	//_listen_socket.close_file_descriptor();
	//_client_socket.close_file_descriptor();
}

/**
 * @brief Creates new TCP/IP server socket
 *
 * @throws tcp_server_exception
 */
void tcp_server::create_socket()
{
	// create ipv4 no blocking socket, with full duplex byte stream communication
	// protocol is chosen by OS
	_listen_socket = socket(PF_INET, SOCK_STREAM, 0); // | SOCK_NONBLOCK, 0);

	if(_listen_socket.get_file_descriptor()<0)
		throw tcp_server_exception{"Error when opening socket.", strerror(errno)};
}

/**
 * @brief Bind socket to machine address and given port
 * @param port Port on which server listens for connections
 * @throws tcp_server_exception
 */
void tcp_server::bind_socket(int port)
{
	sockaddr_in socket_addres;

	// set all values in buffer to a zero
	std::memset(&socket_addres, 0, sizeof(socket_addres));

	// set address family for ipv4
	socket_addres.sin_family = AF_INET;
	// save port number in network byte order
	socket_addres.sin_port = htons(port);
	// set ip address of the host to the address of machine on which server is running
	socket_addres.sin_addr.s_addr = INADDR_ANY;

	int bind_ret_val = bind(_listen_socket.get_file_descriptor(), reinterpret_cast<sockaddr*>(&socket_addres), sizeof(socket_addres));

	if(bind_ret_val < 0 )
		throw tcp_server_exception{"Binding socket to address failed.", strerror(errno)};

}

/**
 * @brief Allow the process to listen on the socket for connections
 * @throws tcp_server_exception
 */
void tcp_server::listen_for_connection()
{
	if(_listen_socket.get_file_descriptor() <= 0)
		throw tcp_server_exception{"Socket isn't valid. Before listening create socket.", strerror(errno)};
	listen(_listen_socket.get_file_descriptor(), 5);
}

/**
 * @brief Waits for incoming connection and accepts it if is available
 */
void tcp_server::reconnect()
{
	if(_client_socket.get_file_descriptor() > 0)
	{
		_client_socket.close_file_descriptor();
		_is_connected = false;
	}
	// accept connection synchronously
	accept_connection();
}


/**
 * @brief Accept connection from client and creates new socket for communication
 * @throws tcp_server_exception
 */
void tcp_server::accept_connection()
{
	std::unique_lock<std::mutex>{_listen_socket.get_mutex()};

	unsigned int client_address_size = sizeof(_client_addres);
	std::memset(&_client_addres, 0, client_address_size);

	std::clog<<"Listening for connection...\n";
	_client_socket = accept(_listen_socket.get_file_descriptor(), reinterpret_cast<sockaddr*>(&_client_addres), &client_address_size);

	if(_client_socket.get_file_descriptor() < 0)
		throw tcp_server_exception("Error when accepting connection.", strerror(errno));
	std::clog<<"Client connected.\n";
	_is_connected = true;
}

/**
 * Called when data is ready to process
 */
void tcp_server::on_data_ready()
{
	read_data();
	if(_is_data_ready_event_subscirbed)
		_data_ready_handler();
}

/**
 * @brief Gets file descriptor
 * @return file descriptor
 */
file_descriptor_handler& tcp_server::get_file_descriptor_handler()
{
	return _client_socket;
}

/**
 * @brief Checks if file descriptor is acquired and can be used
 * @return true if file descriptor is ready to use
 */
bool tcp_server::is_file_descriptor_ready()
{
	return is_connected();
}

/**
 * @brief Send data to connected client
 * @param buffer data buffer
 * @param length data buffer length in bytes
 * @throws tcp_server_exception
 */
void tcp_server::send_data(const std::vector<char>& buffer)
{
	if(_is_connected)
	{
		std::unique_lock<std::mutex>{_client_socket.get_mutex()};

		int length = buffer.size();

		int i = 0;
		for(const char& c:buffer)
		{
			_system_interaction_buffer[i++] = c;
		}

		int written_bytes = send(_client_socket.get_file_descriptor(), _system_interaction_buffer, length, 0);

		if(written_bytes < 0)
			throw tcp_server_exception{"Error when sending data", strerror(errno)};
		if(written_bytes < length)
			std::clog<<"tcp_server:: Less data written than expected. Length: "<<length<<" Bytes count: "<<written_bytes<<"\n";
	}
}


/**
 * @brief Assigns event handler called on data ready event
 * @param data_ready_handler function object with event handler
 */
void tcp_server::subscribe_data_ready_event(delegate&& data_ready_handler)
{
	if(_is_data_ready_event_subscirbed == false)
	{
		std::swap(_data_ready_handler, data_ready_handler);
		_is_data_ready_event_subscirbed = true;
	}
}
/**
 * @brief Unsubscribe data ready event handler
 */
void tcp_server::unsubscribe_data_ready_event()
{
	if(_is_data_ready_event_subscirbed)
	{
		_is_data_ready_event_subscirbed = false;
	}
}

/**
 * @brief Swaps received data from input buffer to given buffer
 * @param buffer buffer to which data will be copied
 */
void tcp_server::receive_data(std::vector<char>& buffer)
{
	std::unique_lock<std::mutex>(_buffer_mutex);
	std::swap(buffer, _received_data_buffer);
	_received_data_buffer.clear();
}

/**
 * @brief Read data from client and write to class buffer
 * @throws tcp_server_exception
 */
void tcp_server::read_data()
{
	if(_is_connected)
	{
		// don't lock yet, wait until we can lock both mutexes to not cause deadlock
		std::unique_lock<std::mutex> fd_lock{_client_socket.get_mutex(), std::defer_lock};
		std::unique_lock<std::mutex> buffer_lock{_buffer_mutex, std::defer_lock};
		std::lock(fd_lock, buffer_lock);


		int bytes_count = recv(_client_socket.get_file_descriptor(), _system_interaction_buffer, _buffer_size, 0);
		if(bytes_count == 0)
		{
			//FIXME This solution makes poll thread exit AFTER new connection is established
			std::clog<<"Client was disconnected.\n";
			_is_connected = false;
			reconnect();
			return;
		}
		if(bytes_count < 0)
			throw tcp_server_exception{"Error reading data from socket", strerror(errno)};

		// copy data from internal buffer to output buffer
		for(int i = 0; i<bytes_count; i++)
		{
			_received_data_buffer.push_back(_system_interaction_buffer[i]);
		}
	}
}
/**
 * @brief Gets connection state
 * @return connection state
 */
bool tcp_server::is_connected() const
{
	return _is_connected;
}


} /* namespace mrobot */

