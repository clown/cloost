/* ------------------------------------------------------------------------- */
/*
 *  example_xauth.cpp
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Fri 22 Oct 2010 20:10:00 JST
 */
/* ------------------------------------------------------------------------- */
#define CLOOST_NET_MIN_LIB
#include <cloost/twitter.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	/*
	 * User settings:
	 * twitter より consumer_key と consumer_secret を取得して，
	 * "xxx" の部分を書き換える．username, password は，twitter
	 * にログオンする際に入力する値である．
	 */
	std::string consumer_key    = "xxx";
	std::string consumer_secret = "xxx";
	std::string username        = "xxx";
	std::string password        = "xxx";
	
	boost::asio::io_service service;
	boost::asio::ssl::context ctx(service, boost::asio::ssl::context::sslv23);
	//ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
	//ctx.load_verify_file("ca.pem");
	
	cloost::twitter::xauth session(consumer_key, consumer_secret, service, ctx);
	
	cloost::http::response res = session.get_access_token(username, password);
	std::cout << "status: " << res.status() << std::endl;
	std::cout << res.body() << std::endl;
	
	cloost::twitter::xauth::parameter_map params;
	params["status"] = "test";
	res = session.post("/1/statuses/update.json", params);
	std::cout << "status: " << res.status() << std::endl;
	std::cout << res.body() << std::endl;
	
	return 0;
}
