/* ------------------------------------------------------------------------- */
/*
 *  https.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Thu 01 Apr 2010 11:09:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_HTTPS_H
#define CLOOST_HTTPS_H

#include "http.h"
#include "ssl_socket_wrapper.h"

namespace cloost {
	typedef basic_http<cloost::ssl::socket_wrapper> https;
}

#endif // CLOOST_HTTPS_H
