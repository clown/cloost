/* ------------------------------------------------------------------------- */
/*
 *  xauth.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Fri 20 Oct 2010 16:06:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_XAUTH_H
#define CLOOST_XAUTH_H

#include "oauth_base.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <clx/salgorithm.h>
#include <clx/uri.h>

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  basic_xauth
	/* --------------------------------------------------------------------- */
	template <class Http, class DomainTraits>
	class basic_xauth : public basic_oauth_base<Http, DomainTraits> {
	public:
		typedef typename basic_oauth_base<Http, DomainTraits>::char_type char_type;
		typedef typename basic_oauth_base<Http, DomainTraits>::string_type string_type;
		typedef typename basic_oauth_base<Http, DomainTraits>::parameter_map parameter_map;
		typedef typename basic_oauth_base<Http, DomainTraits>::http http;
		typedef typename basic_oauth_base<Http, DomainTraits>::traits traits;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_xauth(const string_type& consumer_key, const string_type& consumer_secret,
			boost::asio::io_service& service, boost::asio::ssl::context& ctx) :
			basic_oauth_base<Http, DomainTraits>(consumer_key, consumer_secret, service, ctx) {}
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		virtual ~basic_xauth() throw() {}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  get_access_token
		 *
		 *  the code is derived form the following URL:
		 *  http://mattn.kaoriya.net/software/lang/c/20100911222528.htm
		 */
		/* ----------------------------------------------------------------- */
		typename http::response get_access_token(const string_type& username, const string_type& password) {
			parameter_map params;
			params["x_auth_mode"] = "client_auth";
			params["x_auth_username"] = username;
			params["x_auth_password"] = password;
			
			typename http::response res = this->post(traits::access_token_path(), params);
			this->parse(res.body());
			return res;
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

#endif // CLOOST_XAUTH_H
