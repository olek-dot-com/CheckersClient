#pragma once
#include <iostream>
#include "NetworkIncludes.h"
#include "Message.h"
#include "NetworkConnection.h"
#include "TSQueue.h"


namespace olc::net {
	template <typename T>
	class NetworkClient
	{
	public:
		NetworkClient()
		{}

		virtual ~NetworkClient()
		{
			// If the client is destroyed, always try and disconnect from server
			disconnect();
		}

	public:
		// Connect to server with hostname/ip-address and port
		bool connect(const std::string& host, const uint16_t port)
		{
			try
			{
				// Resolve hostname/ip-address into tangiable physical address
				asio::ip::tcp::resolver resolver(m_context);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

				// Create connection
				m_connection = std::make_unique<NetworkConnection<T>>(NetworkConnection<T>::Owner::client, m_context, asio::ip::tcp::socket(m_context), m_qMessagesIn);

				// Tell the connection object to connect to server
				m_connection->connectToServer(endpoints);

				// Start Context Thread
				thrContext = std::thread([this]() { m_context.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << "Client Exception: " << e.what() << "\n";
				return false;
			}
			return true;
		}

		// Disconnect from server
		void disconnect()
		{
			// If connection exists, and it's connected then...
			if (isConnected())
			{
				// ...disconnect from server gracefully
				m_connection->disconnect();
			}

			// Either way, we're also done with the asio context...				
			m_context.stop();
			// ...and its thread
			if (thrContext.joinable())
				thrContext.join();

			// Destroy the connection object
			m_connection.release();
		}

		// Check if client is actually connected to a server
		bool isConnected()
		{
			if (m_connection)
				return m_connection->isConnected();
			else
				return false;
		}

	public:
		// Send message to server
		void send(const Message<T>& msg)
		{
			if (isConnected())
				m_connection->send(msg);
		}

		// Retrieve queue of messages from server
		tSQueue<OwnedMessage<T>>& Incoming()
		{
			return m_qMessagesIn;
		}

	protected:
		// asio context handles the data transfer...
		asio::io_context m_context;
		// ...but needs a thread of its own to execute its work commands
		std::thread thrContext;
		// The client has a single instance of a "connection" object, which handles data transfer
		std::unique_ptr<NetworkConnection<T>> m_connection;

	private:
		// This is the thread safe queue of incoming messages from server
		tSQueue<OwnedMessage<T>> m_qMessagesIn;
	};
}
