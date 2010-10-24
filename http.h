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
#include "http_receiver.h"
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
		host_(host), port_(port) {}

namespace cloost {
	namespace net {
		/* ----------------------------------------------------------------- */
		//  basic_http
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		class basic_http : boost::noncopyable {
		public:
			typedef std::size_t size_type;
			typedef char char_type;
			typedef std::basic_string<char_type> string_type;
			typedef cloost::net::http_request request;
			typedef cloost::net::http_response response;
			
			/* ------------------------------------------------------------- */
			//  constructor
			/* ------------------------------------------------------------- */
			basic_http(boost::asio::io_service& service, const string_type& host, const string_type& port) :
				socket_(service), host_(host), port_(port) {}
			
			/* ------------------------------------------------------------- */
			/*
			 *  constructor
			 *
			 *  If you use C++0x, you can also write the following
			 *  descritpion, instead of using Boost.Preprocessor libraries.
			 *
			 *  template <class ...Args>
			 *  basic_http(boost::asio::io_service& service,
			 *      const string_type& host, const string_type& port,
			 *      Args... args) :
			 *      socket_(service, args), version_(1.1),
			 *      host_(host), port_(port) {}
			 */
			/* ------------------------------------------------------------- */
			BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(CLOOST_SOCKET_WRAPPER_MAX_ARITY), CLOOST_HTTP_RRIVATE_CTR_DEF, _)
			
			/* ------------------------------------------------------------- */
			//  destructor
			/* ------------------------------------------------------------- */
			~basic_http() throw() {}
			
			/* ------------------------------------------------------------- */
			//  send
			/* ------------------------------------------------------------- */
			template <class HttpReceiver>
			response send(HttpReceiver f, request& req) {
				socket_.reset(host_, port_);
				
				if (req.host().empty()) req.host(host_);
				if (req.port().empty()) req.port(port_);
				cloost::net::write(socket_, req);
				
				return f(socket_, req);
			}
			
			/* ------------------------------------------------------------- */
			//  send
			/* ------------------------------------------------------------- */
			response send(const request& req) {
				cloost::net::http_receiver f;
				return this->send(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  head
			/* ------------------------------------------------------------- */
			template <class HttpReceiver>
			response head(HttpReceiver f, request& req) {
				req.method(request::head);
				return this->send(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  head
			/* ------------------------------------------------------------- */
			response head(request& req) {
				cloost::net::http_receiver f;
				return this->head(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  head
			/* ------------------------------------------------------------- */
			response head(const string_type& path) {
				request req;
				req.path(path);
				return this->head(req);
			}
			
			/* ------------------------------------------------------------- */
			//  get
			/* ------------------------------------------------------------- */
			template <class HttpReceiver>
			response get(HttpReceiver f, request& req) {
				req.method(request::get);
				return this->send(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  get
			/* ------------------------------------------------------------- */
			response get(request& req) {
				cloost::net::http_receiver f;
				return this->get(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  get
			/* ------------------------------------------------------------- */
			response get(const string_type& path) {
				request req;
				req.path(path);
				return this->get(req);
			}
			
			/* ------------------------------------------------------------- */
			//  post
			/* ------------------------------------------------------------- */
			template <class HttpReceiver>
			response post(HttpReceiver f, request& req) {
				req.method(request::post);
				return this->send(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  post
			/* ------------------------------------------------------------- */
			response post(request& req) {
				cloost::net::http_receiver f;
				return this->post(f, req);
			}
			
			/* ------------------------------------------------------------- */
			//  post
			/* ------------------------------------------------------------- */
			response post(const string_type& path, const string_type& body) {
				request req;
				req.path(path);
				req.body(body);
				return this->post(req);
			}
			
			/* ------------------------------------------------------------- */
			//  accessors
			/* ------------------------------------------------------------- */
			const string_type& host() const { return host_; }
			const string_type& port() const { return port_; }
			
		private:
			SocketWrapper socket_;
			const string_type host_;
			const string_type port_;
		};
	}
	
	typedef cloost::net::basic_http<cloost::tcp::socket_wrapper> http;
}

#endif // CLOOST_HTTP_H
