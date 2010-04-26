/* ------------------------------------------------------------------------- */
/*
 *  ssl_socket_wrapper.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Thu 01 Apr 2010 11:09:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_SSL_SOCKET_WRAPPER_H
#define CLOOST_SSL_SOCKET_WRAPPER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace cloost {
	namespace ssl {
		/* ----------------------------------------------------------------- */
		//  socket_wrapper
		/* ----------------------------------------------------------------- */
		class socket_wrapper {
		public:
			typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_type;
			typedef char char_type;
			typedef std::basic_string<char_type> string_type;
			
			/* ------------------------------------------------------------- */
			//  start
			/* ------------------------------------------------------------- */
			socket_wrapper(boost::asio::io_service& service, boost::asio::ssl::context& ctx) :
				service_(service), socket_(service, ctx) {}
			
			/* ------------------------------------------------------------- */
			//  start
			/* ------------------------------------------------------------- */
			void start(const string_type& host, const string_type& port) {
				// Get a list of endpoints corresponding to the server name.
				boost::asio::ip::tcp::resolver resolver(service_);
				boost::asio::ip::tcp::resolver::query query(host, port);
				boost::asio::ip::tcp::resolver::iterator pos = resolver.resolve(query);
				boost::asio::ip::tcp::resolver::iterator last;
				
				// Try each endpoint until we successfully establish a connection.
				boost::system::error_code error = boost::asio::error::host_not_found;
				while (error && pos != last) {
					socket_.lowest_layer().close();
					socket_.lowest_layer().connect(*pos++, error);
					socket_.handshake(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::client);
				}
				if (error) throw boost::system::system_error(error);
			}
			
			/* ------------------------------------------------------------- */
			//  socket
			/* ------------------------------------------------------------- */
			socket_type& socket() { return socket_; }
			
		private:
			boost::asio::io_service& service_;
			socket_type socket_;
		};
	}
}

#endif // CLOOST_SSL_SOCKET_WRAPPER_H
