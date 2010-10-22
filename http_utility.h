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
 *  Last-modified: Fri 22 Oct 2010 10:56:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTP_UTILITY_H
#define CLOOST_HTTP_UTILITY_H

#include <string>
#include <map>
#include <ostream>
#include <boost/asio.hpp>

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  http_request_header
	/* --------------------------------------------------------------------- */
	typedef std::map<std::string, std::string> http_request_header;
	
	/* --------------------------------------------------------------------- */
	//  http_response
	/* --------------------------------------------------------------------- */
	class http_response {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::map<std::string, std::string> header_map;
		typedef header_map::iterator header_iterator;
		typedef header_map::const_iterator const_header_iterator;
		
		const size_type& status() const { return status_; }
		const header_map& headers() const { return headers_; }
		const string_type& body() const { return body_; }
		
		void status(const size_type& cp) { status_ = cp; }
		void body(const string_type& cp) { body_ = cp; }
		header_map& headers() { return headers_; }
		
	private:
		size_type status_;
		header_map headers_;
		string_type body_;
	};
	
	namespace net {
		/* ----------------------------------------------------------------- */
		//  write
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		void write(SocketWrapper& s, const std::string& method, const std::string& path, const http_request_header& options, const std::string& body = std::string()) {
			boost::asio::streambuf request;
			std::basic_ostream<char> output(&request);
			
			output << boost::format("%s %s HTTP/1.1\r\n") % method % path;
			if (options.find("Host") == options.end()) output << "Host: " << s.host() << "\r\n"; // required element in HTTP 1.1
			for (http_request_header::const_iterator pos = options.begin(); pos != options.end(); ++pos) {
				output << pos->first << ": " << pos->second << "\r\n";
			}
			
			if (!body.empty()) { // for POST method
				output << "Content-Length: " << body.size() << "\r\n";
				output << "\r\n";
				output << body;
			}
			else output << "\r\n";
			
			boost::asio::write(s.socket(), request);
		}
		
		/* ----------------------------------------------------------------- */
		//  write
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		void write(SocketWrapper& s, const std::string& method, const std::string& path, const std::string& body) {
			http_request_header dummy;
			cloost::net::write(s, method, path, dummy, body);
		}
		
		/* ----------------------------------------------------------------- */
		//  write
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		void write(SocketWrapper& s, const std::string& method, const std::string& path) {
			http_request_header dummy_opt;
			std::string dummy_body;
			cloost::net::write(s, method, path, dummy_opt, dummy_body);
		}
	}
}

#endif // CLOOST_HTTP_UTILITY_H
