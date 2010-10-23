/* ------------------------------------------------------------------------- */
/*
 *  oauth_base.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Fri 20 Oct 2010 19:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_OAUTH_BASE_H
#define CLOOST_OAUTH_BASE_H

#include "https.h"
#include <sstream>
#include <string>
#include <map>
#include <boost/ref.hpp>
#include <boost/noncopyable.hpp>
#include <clx/base64.h>
#include <clx/hexdump.h>
#include <clx/date_time.h>
#include <clx/format.h>
#include <clx/hmac.h>
#include <clx/sha1.h>
#include <clx/uri.h>

namespace cloost {
	/* --------------------------------------------------------------------- */
	//  basic_oauth_base
	/* --------------------------------------------------------------------- */
	template <class DomainTraits>
	class basic_oauth_base : boost::noncopyable {
	public:
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::map<string_type, string_type> parameter_map;
		typedef DomainTraits traits;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		basic_oauth_base(boost::asio::io_service& service, boost::asio::ssl::context& ctx,
			const string_type& consumer_key, const string_type& consumer_secret) :
			session_(service, traits::domain(), traits::port(), boost::ref(ctx)),
			consumer_key_(consumer_key), consumer_secret_(consumer_secret),
			oauth_token_(), oauth_token_secret_(), timestamp_(), nonce_() {
			clx::date_time now;
			timestamp_ = clx::str(clx::format("%s") % now.c_time());
			nonce_ = clx::hexdump(timestamp_);
		}
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		virtual ~basic_oauth_base() throw() {}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  post
		 *
		 *  The function is derived form the following URL:
		 *  http://mattn.kaoriya.net/software/lang/c/20100911222528.htm
		 *
		 */
		/* ----------------------------------------------------------------- */
		http_response post(const string_type& path, const parameter_map& parameters) {
			parameter_map v = this->merge(parameters);
			
			std::basic_stringstream<char_type> ss;
			bool first = true;
			for (parameter_map::const_iterator pos = v.begin(); pos != v.end(); ++pos) {
				if (!first) ss << '&';
				else first = false;
				ss << pos->first << '=' << pos->second;
			}
			
			string_type key = consumer_secret_ + "&" + oauth_token_secret_;
			string_type uri = traits::protocol() + string_type("://") + traits::domain();
			string_type port = traits::port();
			if (port != "80" && port != "443") uri += ":" + port;
			uri += path;
			string_type val = "POST&" + encode(uri) + "&" + encode(ss.str());
			
			const clx::sha1& hmac = clx::hmac<clx::sha1>(key.c_str(), key.size(), val.c_str(), val.size());
			string_type hm = clx::base64::encode(reinterpret_cast<const char*>(hmac.code()), 20);
			string_type sig = encode(hm);
			ss << "&oauth_signature=" << sig;
			
			http_request_header options;
			options["content-type"] = "application/x-www-form-urlencoded";
			return session_.post(clx::uri::encode(path), ss.str(), options);
		}
		
		/* ----------------------------------------------------------------- */
		//  other access methods, future works...zzZ
		/* ----------------------------------------------------------------- */
		// http_response get();
		
		/* ----------------------------------------------------------------- */
		//  access methods
		/* ----------------------------------------------------------------- */
		const string_type& consumer_key() const { return consumer_key_; }
		const string_type& consumer_secret() const { return consumer_secret_; }
		const string_type oauth_token() const { return oauth_token_; }
		const string_type oauth_token_secret() const { return oauth_token_secret_; }
		
		void consumer_key(const string_type& cp) { consumer_key_ = cp; }
		void consumer_secret(const string_type& cp) { consumer_secret_ = cp; }
		void oauth_token(const string_type& cp) { oauth_token_ = cp; }
		void oauth_token_secret(const string_type& cp) { oauth_token_secret_ = cp; }
		
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		static string_type encode(const string_type& src) {
			static clx::uri_encoder f("-._~", false, false);
			return clx::convert(src, f);
		}
		
	protected:
		cloost::https& session() { return session_; }
		
	private:
		cloost::https session_;
		
		// oauth user settings
		string_type consumer_key_;
		string_type consumer_secret_;
		string_type oauth_token_;
		string_type oauth_token_secret_;
		string_type timestamp_;
		string_type nonce_;
		
		/* ----------------------------------------------------------------- */
		//  merge
		/* ----------------------------------------------------------------- */
		parameter_map merge(const parameter_map& users) {
			parameter_map dest;
			dest["oauth_consumer_key"] = consumer_key_;
			dest["oauth_nonce"] = nonce_;
			dest["oauth_signature_method"] = "HMAC-SHA1";
			dest["oauth_timestamp"] = timestamp_;
			dest["oauth_version"] = "1.0";
			if (!oauth_token_.empty()) dest["oauth_token"] = oauth_token_;
			
			for (parameter_map::const_iterator pos = users.begin(); pos != users.end(); ++pos) {
				dest.insert(*pos);
			}
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  authorization_header
		/* ----------------------------------------------------------------- */
		string_type authorization_header(const string_type& sig) {
			parameter_map users;
			users["oauth_signature"] = sig;
			parameter_map params = this->merge(users);
			
			string_type uri = traits::protocol() + string_type("://") + traits::domain() + "/";
			std::basic_stringstream<char_type> ss;
			ss << "OAuth realm=\"" << uri << "\"";
			for (parameter_map::const_iterator pos = params.begin(); pos != params.end(); ++pos) {
				ss << ',';
				ss << pos->first << "=\"" << pos->second << "\"";
			}
			return ss.str();
		}
	};
}

#endif // CLOOST_OAUTH_BASE_H
