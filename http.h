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
 *  Last-modified: Fri 20 Oct 2010 11:20:00 JST
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
#include "http_utility.h"
#include "http_response_reader.h"
#include "tcp_socket_wrapper.h"

/* ------------------------------------------------------------------------- */
//  for variadic templates in C++03
/* ------------------------------------------------------------------------- */
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>

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
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_http(boost::asio::io_service& service, const string_type& host, const string_type& port) :
			socket_(service), version_(1.1), host_(host), port_(port) {}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  constructor
		 *
		 *  If you use C++0x, you can also write the following descritpion,
		 *  instead of using Boost.Preprocessor libraries.
		 *
		 *  template <class ...Args>
		 *  basic_http(boost::asio::io_service& service,
		 *      const string_type& host, const string_type& port,
		 *      Args... args) :
		 *      socket_(service, args), version_(1.1),
		 *      host_(host), port_(port) {}
		 */
		/* ----------------------------------------------------------------- */
		BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(CLOOST_SOCKET_WRAPPER_MAX_ARITY), CLOOST_HTTP_RRIVATE_CTR_DEF, _)
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		~basic_http() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  head
		/* ----------------------------------------------------------------- */
		http_response head(const string_type& path, const http_request_header& options = http_request_header()) {
			socket_.reset(host_, port_);
			cloost::net::write(socket_, "HEAD", path, options, string_type());
			cloost::http_response_reader f;
			return f(socket_);
		}
		
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		template <class ResponseReader>
		http_response get(ResponseReader f, const string_type& path, const http_request_header& options = http_request_header()) {
			socket_.reset(host_, port_);
			cloost::net::write(socket_, "GET", path, options, string_type());
			return f(socket_);
		}
		
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		http_response get(const string_type& path, const http_request_header& options = http_request_header()) {
			cloost::http_response_reader f;
			return this->get(f, path, options);
		}
		
		/* ----------------------------------------------------------------- */
		//  post
		/* ----------------------------------------------------------------- */
		template <class ResponseReader>
		http_response post(ResponseReader f, const string_type& path, const string_type& body, const http_request_header& options = http_request_header()) {
			socket_.reset(host_, port_);
			cloost::net::write(socket_, "POST", path, options, body);
			return f(socket_);
		}
		
		/* ----------------------------------------------------------------- */
		//  post
		/* ----------------------------------------------------------------- */
		http_response post(const string_type& path, const string_type& body, const http_request_header& options = http_request_header()) {
			cloost::http_response_reader f;
			return this->post(f, path, body, options);
		}
		
		/* ----------------------------------------------------------------- */
		//  accessors
		/* ----------------------------------------------------------------- */
		double version() const { return version_; }
		const string_type& hostname() const { return host_; }
		const string_type& port() const { return port_; }
		
	private:
		SocketWrapper socket_;
		double version_;
		const string_type host_;
		const string_type port_;
	};
	
	typedef basic_http<cloost::tcp::socket_wrapper> http;
}

#endif // CLOOST_HTTP_H
