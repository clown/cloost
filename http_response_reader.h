/* ------------------------------------------------------------------------- */
/*
 *  http_response_reader.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Mon 26 Apr 2010 14:20:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTP_RESPONSE_READER_H
#define CLOOST_HTTP_RESPONSE_READER_H

#include <cstdlib>
#include <istream>
#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include "http_utility.h"

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  http_response_reader
	/* --------------------------------------------------------------------- */
	class http_response_reader {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		
		/* ----------------------------------------------------------------- */
		//  operator()
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper>
		http_response operator()(SocketWrapper& s) {
			http_response dest;
			boost::asio::streambuf buf;
			this->read_header(s, buf, dest);
			this->read_body(s, buf, dest);
			return dest;
		}
		
	private:
		/* ----------------------------------------------------------------- */
		//  read_header
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper, class MutableBuffer>
		void read_header(SocketWrapper& s, MutableBuffer& buf, http_response& dest) {
			std::basic_istream<char_type> input(&buf);
			
			// check status code.
			boost::asio::read_until(s.socket(), buf, "\r\n");
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
			boost::asio::read_until(s.socket(), buf, "\r\n\r\n");
			string_type header;
			string_type key;
			while (std::getline(input, header) && !header.empty() && header != "\r") {
				if (header.at(0) == 0x09 || header.at(0) == 0x20) {
					http_response::header_iterator pos = dest.headers().find(key);
					if (pos == dest.headers().end()) std::runtime_error("invalid http response");
					pos->second += boost::trim_copy(header);
				}
				else {
					const size_type pos = header.find_first_of(':');
					if (pos == string_type::npos) std::runtime_error("invalid http response");
					key = header.substr(0, pos);
					const string_type value = boost::trim_copy(header.substr(pos + 1));
					dest.headers().insert(std::make_pair(key, value));
				}
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  read_body
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper, class MutableBuffer>
		void read_body(SocketWrapper& s, MutableBuffer& buf, http_response& dest) {
			http_response::const_header_iterator pos;
			if ((pos = dest.headers().find("Transfer-Encoding")) != dest.headers().end() && pos->second == "chunked") {
				dest.body(this->read_chunked_data(s, buf));
			}
			else if ((pos = dest.headers().find("Content-Length")) != dest.headers().end()) {
				const size_type n = boost::lexical_cast<size_type>(pos->second);
				dest.body(this->read_data(s, buf, n));
			}
			else dest.body(this->read_data(s, buf));
		}
		
		/* ----------------------------------------------------------------- */
		//  read_data
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper, class MutableBuffer>
		string_type read_data(SocketWrapper& s, MutableBuffer& buf) {
			boost::system::error_code error;
			while (boost::asio::read(s.socket(), buf, boost::asio::transfer_at_least(1), error));
			if (error != boost::asio::error::eof) throw boost::system::system_error(error);
			
			const string_type dest(boost::asio::buffer_cast<const char_type*>(buf.data()), buf.size());
			buf.consume(buf.size());
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  read_data
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper, class MutableBuffer>
		string_type read_data(SocketWrapper& s, MutableBuffer& buf, size_type n) {
			if (n == 0) return string_type();
			
			if (buf.size() < n) {
				boost::system::error_code error;
				boost::asio::read(s.socket(), buf, boost::asio::transfer_at_least(n - buf.size()), error);
			}
			if (buf.size() < n) throw std::runtime_error("too few data");
			
			const string_type dest(boost::asio::buffer_cast<const char_type*>(buf.data()), n);
			buf.consume(n);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  read_chunked_data
		/* ----------------------------------------------------------------- */
		template <class SocketWrapper, class MutableBuffer>
		string_type read_chunked_data(SocketWrapper& s, MutableBuffer& buf) {
			string_type dest;
			std::basic_istream<char_type> input(&buf);
			
			while (1) {
				boost::asio::read_until(s.socket(), buf, "\r\n");
				string_type line;
				if (!std::getline(input, line)) throw std::runtime_error("invalid chunked data");
				boost::trim_right(line);
				if (line.empty()) continue;
				
				const size_type n = std::strtol(line.c_str(), NULL, 16);
				if (n == 0) break;
				dest += this->read_data(s, buf, n);
			}
			return dest;
		}
	};
}

#endif // CLOOST_HTTP_RESPONSE_READER_H
