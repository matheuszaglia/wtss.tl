#
#  Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of the WTSS.CXX.
#
#  WTSS.CXX is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 3 as
#  published by the Free Software Foundation.
#
#  WTSS.CXX is distributed  "AS-IS" in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY OF ANY KIND; without even the implied warranty
#  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with WTSS.CXX. See COPYING. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
#
#
#  CMake scripts for WTSS.CXX
#
#  Author: Matheus Cavassan Zaglia <mzaglia@dpi.inpe.br>
#
#  Description: Find WTSS.CXX include directory.
#
#  RAPIDJSON_INCLUDE_DIR - where to find wtss-cxx/wtss.hpp and other header files.
#  RAPIDJSON_FOUND       - True if WTSS.CXX found.
#

find_path(wtss_cxx_client_INCLUDE_DIR wtss-cxx/wtss.hpp
          PATHS /usr
                /usr/local
          PATH_SUFFIXES include
                        include/wtss-cxx)

find_library(wtss_cxx_client_LIBRARY
            NAMES wtss_cxx_client
            PATHS /usr
                  /usr/local
            PATH_SUFFIXES
            lib
            )
include(FindPackageHandleStandardArgs)

set(wtss_cxx_client ${wtss_cxx_client_LIBRARY})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(wtts_cxx_client DEFAULT_MSG wtss_cxx_client_INCLUDE_DIR wtss_cxx_client_LIBRARY)

mark_as_advanced(wtss_cxx_client_INCLUDE_DIR wtss_cxx_client_LIBRARY)

if(wtss_cxx_client_FOUND)
  mark_as_advanced(wtss_cxx_client_ROOT)
endif()
