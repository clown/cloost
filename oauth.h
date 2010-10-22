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
	template <class DomainTraits>
	class basic_oauth : public basic_oauth_base<DomainTraits> {
	public:
		typedef typename basic_oauth_base<DomainTraits>::char_type char_type;
		typedef typename basic_oauth_base<DomainTraits>::string_type string_type;
		typedef typename basic_oauth_base<DomainTraits>::parameter_map parameter_map;
		typedef typename basic_oauth_base<DomainTraits>::traits traits;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_oauth(boost::asio::io_service& service, boost::asio::ssl::context& ctx,
			const string_type& consumer_key, const string_type& consumer_secret) :
			basic_oauth_base<DomainTraits>(service, ctx, consumer_key, consumer_secret) {}
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		virtual ~basic_oauth() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  get_request_token
		/* ----------------------------------------------------------------- */
		http_response get_request_token() {
			http_response response = this->post(traits::request_token_path(), parameter_map());
			this->parse(response.body());
			return response;
		}
		
		/* ----------------------------------------------------------------- */
		//  get_access_token
		/* ----------------------------------------------------------------- */
		http_response get_access_token(const string_type& pin) {
			parameter_map params;
			params["oauth_verifier"] = pin;
			http_response response = this->post(traits::access_token_path(), params);
			this->parse(response.body());
			return response;
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
