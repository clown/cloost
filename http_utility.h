/* ------------------------------------------------------------------------- */
/*
 *  http_utility.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Mon 25 Oct 2010 02:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTP_UTILITY_H
#define CLOOST_HTTP_UTILITY_H

#include "config.h"
#include "http_request.h"
#include "http_response.h"
#include <ostream>
#include <boost/asio.hpp>
#include <boost/format.hpp>

namespace cloost {
	namespace net {
		/* ----------------------------------------------------------------- */
		//  write
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		void write(SocketWrapper& s, const http_request& request) {
			boost::asio::streambuf buffer;
			std::basic_ostream<char> output(&buffer);
			
			output << boost::format("%s %s HTTP/%s\r\n")
				% cloost::net::http_request::str(request.method()) % request.path() % request.version();
			
			// Host header is required element in HTTP 1.1
			if (request.headers().find("Host") == request.headers().end() && !request.host().empty()) {
				output << "Host: " << request.host() << "\r\n";
			}
			
			for (http_request::header_map::const_iterator pos = request.headers().begin();
				pos != request.headers().end(); ++pos) {
				output << pos->first << ": " << pos->second << "\r\n";
			}
			
			if (!request.body().empty()) { // for POST method
				if (request.headers().find("Content-Length") == request.headers().end()) {
					output << "Content-Length: " << request.body().size() << "\r\n";
				}
				output << "\r\n";
				output << request.body();
			}
			else output << "\r\n";
			
			boost::asio::write(s.socket(), buffer);
		}
	}
}

#endif // CLOOST_HTTP_UTILITY_H
