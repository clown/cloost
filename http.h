/* ------------------------------------------------------------------------- */
/*
 *  http.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Tue 27 Apr 2010 04:04:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTP_H
#define CLOOST_HTTP_H

#include "config.h"
#include <istream>
#include <ostream>
#include <string>
#include <map>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include "http_response.h"
#include "http_response_reader.h"
#include "tcp_socket_wrapper.h"

/* ------------------------------------------------------------------------- */
//  for variadic templates in C++03
/* ------------------------------------------------------------------------- */
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

#define CLOOST_HTTP_MAX_ARITY 1
#define CLOOST_HTTP_RRIVATE_CTR_DEF(z, n, data) \
	template <BOOST_PP_ENUM_PARAMS(n, typename T)> \
	basic_http(boost::asio::io_service& service, const string_type& host, const string_type& port, \
		BOOST_PP_ENUM_BINARY_PARAMS(n, T, x)) : \
		socket_(service, BOOST_PP_ENUM_PARAMS(n, x)), \
		version_(1.1), host_(host), port_(port) {}

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  basic_http
	/* --------------------------------------------------------------------- */
	template <class SocketWrapper>
	class basic_http : boost::noncopyable {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::map<string_type, string_type> header_map;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_http(boost::asio::io_service& service, const string_type& host, const string_type& port) :
			socket_(service), version_(1.1), host_(host), port_(port) {}
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(CLOOST_HTTP_MAX_ARITY), CLOOST_HTTP_RRIVATE_CTR_DEF, _)
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		~basic_http() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  head
		/* ----------------------------------------------------------------- */
		http_response head(const string_type& path, const header_map& options = header_map()) {
			this->write_request("HEAD", path, options, string_type());
			
			http_response dest;
			boost::asio::streambuf buffer;
			this->read_response_header(buffer, dest);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		template <class ResponseReader>
		http_response get(ResponseReader f, const string_type& path, const header_map& options = header_map()) {
			this->write_request("GET", path, options, string_type());
			
			http_response dest;
			boost::asio::streambuf buffer;
			this->read_response_header(buffer, dest);
			f(socket_.socket(), buffer, dest);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		http_response get(const string_type& path, const header_map& options = header_map()) {
			cloost::http_response_reader f;
			return this->get(f, path, options);
		}
		
		/* ----------------------------------------------------------------- */
		//  post
		/* ----------------------------------------------------------------- */
		template <class ResponseReader>
		http_response post(ResponseReader f, const string_type& path, const string_type& body, const header_map& options = header_map()) {
			this->write_request("POST", path, options, body);
			
			http_response dest;
			boost::asio::streambuf buffer;
			this->read_response_header(buffer, dest);
			f(socket_.socket(), buffer, dest);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  post
		/* ----------------------------------------------------------------- */
		http_response post(const string_type& path, const string_type& body, const header_map& options = header_map()) {
			cloost::http_response_reader f;
			return this->post(f, path, body, options);
		}
		
		/* ----------------------------------------------------------------- */
		//  accessors
		/* ----------------------------------------------------------------- */
		double version() const { return version_; }
		void version(double cp) { version_ = cp; }
		const string_type& hostname() const { return host_; }
		const string_type& port() const { return port_; }
		
	private:
		SocketWrapper socket_;
		double version_;
		string_type host_;
		string_type port_;
		
		/* ----------------------------------------------------------------- */
		//  write_request
		/* ----------------------------------------------------------------- */
		void write_request(const string_type& method, const string_type& path, const header_map& options, const string_type& body) {
			boost::asio::streambuf request;
			std::basic_ostream<char_type> output(&request);
			
			output << boost::format("%s %s HTTP/%.1f\r\n") % method % path % version_;
			output << "Host: " << host_ << "\r\n"; // required element in HTTP 1.1
			for (header_map::const_iterator pos = options.begin(); pos != options.end(); ++pos) {
				output << pos->first << ": " << pos->second << "\r\n";
			}
			
			if (!body.empty()) { // for POST method
				output << "Content-Length: " << body.size() << "\r\n";
				output << "\r\n";
				output << body;
			}
			else output << "\r\n";
			
			socket_.start(host_, port_);
			boost::asio::write(socket_.socket(), request);
		}
		
		/* ----------------------------------------------------------------- */
		//  read_response_header
		/* ----------------------------------------------------------------- */
		template <class MutableBuffer>
		void read_response_header(MutableBuffer& buf, http_response& dest) {
			std::basic_istream<char_type> input(&buf);
			
			// check status code.
			boost::asio::read_until(socket_.socket(), buf, "\r\n");
			string_type version;
			input >> version;
			size_type code;
			input >> code;
			string_type message;
			std::getline(input, message);
			if (!input || version.substr(0, 5) != "HTTP/") {
				throw std::runtime_error("invalid http response");
			}
			dest.status(code);
			
			// parse response header
			boost::asio::read_until(socket_.socket(), buf, "\r\n\r\n");
			string_type header;
			while (std::getline(input, header) && header != "\r") {
				const size_type pos = header.find_first_of(':');
				if (pos == string_type::npos) continue;
				const string_type key = header.substr(0, pos);
				const string_type value = boost::trim_copy(header.substr(pos + 1));
				dest.headers().insert(std::make_pair(key, value));
			}
		}
	};
	
	typedef basic_http<cloost::tcp::socket_wrapper> http;
}

#endif // CLOOST_HTTP_H
