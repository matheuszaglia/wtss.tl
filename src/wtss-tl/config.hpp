/*
  Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.

  This file is part of the WTSS.TL.

  WTSS.TL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  WTSS.TL is distributed  "AS-IS" in the hope that it will be useful,
  but WITHOUT ANY WARRANTY OF ANY KIND; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along
  with WTSS.TL. See COPYING. If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/config.hpp

  \brief Proxy file for the real wtss_tl_config.hpp file.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_TL_CONFIG_HPP__
#define __WTSS_TL_CONFIG_HPP__

#define TE_QT_WTSS_PLUGIN_NAME "te.qt.wtss"

#ifdef WIN32
  #ifdef WTSSTLDLL
    #define WTSSTLEXPORT __declspec(dllexport)
  #else
    #define WTSSTLEXPORT __declspec(dllimport)
  #endif
#else
  #define WTSSTLEXPORT
#endif

#endif  // __WTSS_TL_CONFIG_HPP__
