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

#include <string>
#include <map>
#include <ostream>
#include <boost/asio.hpp>

namespace cloost {
	namespace net {
		/* ----------------------------------------------------------------- */
		/*
		 *  http_method
		 *
		 */
		/* ----------------------------------------------------------------- */
		struct http_method {
		};
		
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
		
		/* ----------------------------------------------------------------- */
		//  http_response
		/* ----------------------------------------------------------------- */
		class http_response {
		public:
			typedef std::size_t size_type;
			typedef char char_type;
			typedef std::basic_string<char_type> string_type;
			typedef std::map<std::string, std::string> header_map;
			typedef header_map::iterator header_iterator;
			typedef header_map::const_iterator const_header_iterator;
			
			http_response() : status_(0), headers_(), body_() {}
			
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
		
		/* ----------------------------------------------------------------- */
		/*
		 *  str
		 *
		 *  Get HTTP Method string in order to send messages
		 *  to HTTP servers.
		 */
		/* ----------------------------------------------------------------- */
		inline std::string str(const http_request::method_type& method) {
			switch (method) {
			case http_request::head:     return "HEAD";
			case http_request::get:      return "GET";
			case http_request::post:     return "POST";
			case http_request::put:      return "PUT";
			case http_request::del:      return "DELETE";
			case http_request::options:  return "OPTIONS";
			case http_request::trace:    return "TRACE";
			case http_request::connect:  return "CONNECT";
			case http_request::patch:    return "PATCH";
			case http_request::link:     return "LINK";
			case http_request::unlink:   return "UNLINK";
			default: break;
			}
			return "UNKNOWN";
		}
		
		/* ----------------------------------------------------------------- */
		//  write
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		void write(SocketWrapper& s, const http_request& request) {
			boost::asio::streambuf buffer;
			std::basic_ostream<char> output(&buffer);
			
			output << boost::format("%s %s HTTP/%s\r\n")
				% cloost::net::str(request.method()) % request.path() % request.version();
			
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
