/* ------------------------------------------------------------------------- */
/*
 *  example_oauth_base.cpp
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
#define CLOOST_NET_MIN_LIB
#include <cloost/twitter.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	if (argc < 2) return -1;
	
	/*
	 * User settings:
	 * twitter より consumer_key と consumer_secret を取得して，
	 * "xxx" の部分を書き換える．また，access_token, access_token_secret は
	 * example-oauth.cpp などを利用して実際に OAuth 通信を（途中まで）
	 * 行って取得する必要がある．
	 */
	std::string consumer_key        = "xxx";
	std::string consumer_secret     = "xxx";
	std::string access_token        = "xxx";
	std::string access_token_secret = "xxx";
	
	boost::asio::io_service service;
	boost::asio::ssl::context ctx(service, boost::asio::ssl::context::sslv23);
	//ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
	//ctx.load_verify_file("ca.pem");
	
	cloost::twitter::oauth_base session(consumer_key, consumer_secret, service, ctx);
	session.oauth_token(access_token);
	session.oauth_token_secret(access_token_secret);
	
	cloost::twitter::oauth_base::parameter_map params;
	params["status"] = cloost::twitter::oauth_base::encode(argv[1]);
	cloost::http::response res = session.post("/1/statuses/update.json", params);
	
	std::cout << "status: " << res.status() << std::endl;
	//std::cout << res.body() << std::endl;
	
	return 0;
}
