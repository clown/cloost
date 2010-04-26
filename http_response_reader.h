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
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include "http_response.h"

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
		template <class SyncReadStream, class MutableBuffers>
		void operator()(SyncReadStream& s, MutableBuffers& buf, http_response& response) {
			if (response.headers().find("Transfer-Encoding") != response.headers().end() &&
				boost::to_lower_copy(response.headers().find("Transfer-Encoding")->second) == "chunked") {
				response.body(this->read_chunked_data(s, buf));
			}
			else if (response.headers().find("Content-Length") != response.headers().end()) {
				const size_type n = boost::lexical_cast<size_type>(response.headers().find("Content-Length")->second);
				response.body(this->read_data(s, buf, n));
			}
			else response.body(this->read_data(s, buf));
		}
		
	private:
		/* ----------------------------------------------------------------- */
		//  read_data
		/* ----------------------------------------------------------------- */
		template <class SyncReadStream, class MutableBuffers>
		string_type read_data(SyncReadStream& s, MutableBuffers& buf) {
			boost::system::error_code error;
			while (boost::asio::read(s, buf, boost::asio::transfer_at_least(1), error));
			if (error != boost::asio::error::eof) throw boost::system::system_error(error);
			
			const string_type dest(boost::asio::buffer_cast<const char_type*>(buf.data()), buf.size());
			buf.consume(buf.size());
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  read_data
		/* ----------------------------------------------------------------- */
		template <class SyncReadStream, class MutableBuffers>
		string_type read_data(SyncReadStream& s, MutableBuffers& buf, size_type n) {
			if (buf.size() < n) {
				boost::system::error_code error;
				boost::asio::read(s, buf, boost::asio::transfer_at_least(n - buf.size()), error);
			}
			if (buf.size() < n) throw std::runtime_error("too few data");
			
			const string_type dest(boost::asio::buffer_cast<const char_type*>(buf.data()), n);
			buf.consume(n);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  read_chunked_data
		/* ----------------------------------------------------------------- */
		template <class SyncReadStream, class MutableBuffers>
		string_type read_chunked_data(SyncReadStream& s, MutableBuffers& buf) {
			string_type dest;
			std::basic_istream<char_type> input(&buf);
			
			while (1) {
				boost::asio::read_until(s, buf, "\r\n");
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
