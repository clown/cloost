#ifndef CLOOST_TWITTER_H
#define CLOOST_TWITTER_H

#include "oauth_base.h"
#include "oauth.h"
#include "xauth.h"

namespace cloost {
	struct twitter_traits {
		typedef char char_type;
		
		static const char_type* protocol() { return "https"; }
		static const char_type* domain() { return "api.twitter.com"; }
		static const char_type* port() { return "443"; }
		static const char_type* request_token_path() { return "/oauth/request_token"; }
		static const char_type* access_token_path() { return "/oauth/access_token"; }
		static const char_type* authorize_path() { return "/oauth/authorize"; }
	};
	
	namespace twitter {
		typedef cloost::basic_oauth_base<cloost::twitter_traits> oauth_base;
		typedef cloost::basic_oauth<cloost::twitter_traits> oauth;
		typedef cloost::basic_xauth<cloost::twitter_traits> xauth;
	}
}

#endif // CLOOST_TWITTER_H
