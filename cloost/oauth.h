/* ------------------------------------------------------------------------- */
/*
 *  oauth.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Fri 20 Oct 2010 16:18:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_OAUTH_H
#define CLOOST_OAUTH_H

#include "oauth_base.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <clx/salgorithm.h>
#include <clx/uri.h>

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  constructor
	/* --------------------------------------------------------------------- */
	template <class Http, class DomainTraits>
	class basic_oauth : public basic_oauth_base<Http, DomainTraits> {
	public:
		typedef typename basic_oauth_base<Http, DomainTraits>::char_type char_type;
		typedef typename basic_oauth_base<Http, DomainTraits>::string_type string_type;
		typedef typename basic_oauth_base<Http, DomainTraits>::parameter_map parameter_map;
		typedef typename basic_oauth_base<Http, DomainTraits>::http http;
		typedef typename basic_oauth_base<Http, DomainTraits>::traits traits;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_oauth(const string_type& consumer_key, const string_type& consumer_secret,
			boost::asio::io_service& service, boost::asio::ssl::context& ctx) :
			basic_oauth_base<Http, DomainTraits>(consumer_key, consumer_secret, service, ctx) {}
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		virtual ~basic_oauth() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  get_request_token
		/* ----------------------------------------------------------------- */
		typename http::response get_request_token() {
			typename http::response res = this->post(traits::request_token_path(), parameter_map());
			this->parse(res.body());
			return res;
		}
		
		/* ----------------------------------------------------------------- */
		//  get_access_token
		/* ----------------------------------------------------------------- */
		typename http::response get_access_token(const string_type& pin) {
			parameter_map params;
			params["oauth_verifier"] = pin;
			typename http::response res = this->post(traits::access_token_path(), params);
			this->parse(res.body());
			return res;
		}
		
		/* ----------------------------------------------------------------- */
		//  authorize_uri
		/* ----------------------------------------------------------------- */
		string_type authorize_uri() const {
			string_type dest = traits::protocol() + string_type("://") + traits::domain() + traits::authorize_path();
			dest += string_type("?oauth_token=") + this->oauth_token();
			return dest;
		}
		
	private:
		/* ----------------------------------------------------------------- */
		//  parse
		/* ----------------------------------------------------------------- */
		void parse(const string_type& body) {
			std::vector<std::string> item;
			parameter_map params;
			clx::split_if(body, item, clx::is_any_of(LITERAL("&")));
			for (unsigned int i = 0; i < item.size(); i++) {
				std::string token = item.at(i);
				size_t pos = token.find_first_of("=");
				params[token.substr(0, pos)] = clx::uri::decode(token.substr(pos+1));
			}
			
			this->oauth_token(params["oauth_token"]);
			this->oauth_token_secret(params["oauth_token_secret"]);
		}
	};
}

#endif // CLOOST_OAUTH_H
