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
#ifndef CLOOST_HTTP_RESPONSE_H
#define CLOOST_HTTP_RESPONSE_H

#include "config.h"
#include <string>
#include <map>

namespace cloost {
	namespace net {
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
	}
}

#endif // CLOOST_HTTP_RESPONSE_H
