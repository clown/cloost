/* ------------------------------------------------------------------------- */
/*
 *  config.h
 *
 *  Copyright (c) 2010, clown. All rights reserved.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Tue 27 Apr 2010 16:27:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLOOST_CONFIG_H
#define CLOOST_CONFIG_H

/* ------------------------------------------------------------------------- */
//  configuration for network libraries
/* ------------------------------------------------------------------------- */
#define CLOOST_SOCKET_WRAPPER_MAX_ARITY 4

#ifdef CLOOST_NETWORK_MINIMUM_SET
#define BOOST_REGEX_NO_LIB
#define BOOST_DATE_TIME_NO_LIB
#endif // CLOOST_NETWORK_MINIMUM_SET

#if defined(__CYGWIN__) || defined(__CYGWIN32__) || defined(__MINGW32__)
#define __USE_W32_SOCKETS
#define _WIN32_WINNT 0x0501
#endif

#endif // CLOOST_CONFIG_H
