/* ------------------------------------------------------------------------- */
/*
 *  http_response.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Mon 26 Apr 2010 12:11:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTP_RESPONSE_H
#define CLOOST_HTTP_RESPONSE_H

#include <string>
#include <map>

namespace cloost {
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
		typedef header_map::const_iterator header_const_iterator;
		
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

#endif // CLOOST_HTTP_RESPONSE_H
