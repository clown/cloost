/* ------------------------------------------------------------------------- */
/*
 *  example_https.cpp
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Mon 26 Apr 2010 19:48:00 JST
 */
/* ------------------------------------------------------------------------- */
#include <cloost/https.h>
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/ref.hpp>

int main(int argc, char* argv[]) {
	if (argc < 3) return -1;
	
	try {
		boost::asio::io_service service;
		boost::asio::ssl::context ctx(service, boost::asio::ssl::context::sslv23);
		//ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
		//ctx.load_verify_file("ca.pem");
		
		cloost::https session(service, argv[1], "443", boost::ref(ctx));
		cloost::http::response res = session.get(argv[2]);
		
		std::cout << "status: " << res.status() << std::endl;
		std::cout << std::endl;
		
		std::cout << "head" << std::endl;
		std::cout << "--" << std::endl;
		for (cloost::http::response::const_header_iterator pos = res.headers().begin();
			pos != res.headers().end(); pos++) {
			std::cout << pos->first << ": " << pos->second << std::endl;
		}
		std::cout << std::endl;
		std::cout << "body size: " << res.body().size() << std::endl;
		//std::cout << res.body() << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(-1);
	}
	
	return 0;
}
