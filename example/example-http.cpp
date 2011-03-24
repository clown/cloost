/* ------------------------------------------------------------------------- */
/*
 *  example_http.cpp
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
#define CLOOST_NET_MIN_LIB
#include <cloost/http.h>
#include <iostream>
#include <exception>
#include <boost/asio.hpp>

int main(int argc, char* argv[]) {
	if (argc < 3) return -1;
	
	try {
		boost::asio::io_service service;
		
		cloost::http session(service, argv[1], "80");
		
		cloost::http::request req;
		req.path(argv[2]);
		cloost::http::request::header_map& options = req.headers();
		options["Connection"] = "close";
		cloost::http::response res = session.get(req);
		
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
		std::cout << res.body() << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::exit(-1);
	}
	
	return 0;
}
