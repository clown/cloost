/* ------------------------------------------------------------------------- */
/*
 *  http_request.h
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
#ifndef CLOOST_HTTP_REQUEST_H
#define CLOOST_HTTP_REQUEST_H

#include "config.h"
#include <string>
#include <map>

namespace cloost {
	namespace net {
		/* ----------------------------------------------------------------- */
		//  http_request
		/* ----------------------------------------------------------------- */
		class http_request {
		public:
			typedef std::size_t size_type;
			typedef char char_type;
			typedef std::basic_string<char_type> string_type;
			typedef std::map<std::string, std::string> header_map;
			typedef header_map::iterator header_iterator;
			typedef header_map::const_iterator const_header_iterator;
			
			/* ------------------------------------------------------------- */
			/*
			 *  method_type
			 *
			 *  The cloost library currently supports only head, get, and post
			 *  methods.
			 */
			/* ------------------------------------------------------------- */
			enum method_type {
				unknown     = 0x00,
				head        = 0x01,
				get         = 0x02,
				post        = 0x03,
				put         = 0x04,
				del         = 0x05,
				options     = 0x06,
				trace       = 0x07,
				connect     = 0x08,
				patch       = 0x09,
				link        = 0x0a,
				unlink      = 0x0b
			};
			
			/* ------------------------------------------------------------- */
			/*
			 *  str
			 *
			 *  Get HTTP Method string in order to send messages
			 *  to HTTP servers.
			 */
			/* ------------------------------------------------------------- */
			static string_type str(const method_type& method) {
				switch (method) {
				case head:     return "HEAD";
				case get:      return "GET";
				case post:     return "POST";
				case put:      return "PUT";
				case del:      return "DELETE";
				case options:  return "OPTIONS";
				case trace:    return "TRACE";
				case connect:  return "CONNECT";
				case patch:    return "PATCH";
				case link:     return "LINK";
				case unlink:   return "UNLINK";
				default: break;
				}
				return "UNKNOWN";
			}
			
			http_request() :
				method_(unknown), version_("1.1"),
				host_(), port_(), path_(), body_(), headers_() {}
			
			const method_type& method() const { return method_; }
			const string_type& version() const { return version_; }
			const string_type& host() const { return host_; }
			const string_type& port() const { return port_; }
			const string_type& path() const { return path_; }
			const string_type& body() const { return body_; }
			const header_map& headers() const { return headers_; }
			
			void method(const method_type& cp) { method_ = cp; }
			void version(const string_type& cp) { version_ = cp; }
			void host(const string_type& cp) { host_ = cp; }
			void port(const string_type& cp) { port_ = cp; }
			void path(const string_type& cp) { path_ = cp; }
			void body(const string_type& cp) { body_ = cp; }
			header_map& headers() { return headers_; }
			
		private:
			method_type method_;
			string_type version_;
			string_type host_;
			string_type port_;
			string_type path_;
			string_type body_;
			header_map headers_;
		};
	}
}

#endif // CLOOST_HTTP_REQUEST_H
