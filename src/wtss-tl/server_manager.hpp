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
  \file wtss-tl/server_manager.hpp

  \brief Brief.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_TL_WTSS_HPP__
#define __WTSS_TL_WTSS_HPP__


#endif // __WTSS_TL_WTSS_HPP__
//STL
#include <string>
//QT
#include <QJsonDocument>

namespace wtss_tl
{
  class server_manager
  {
    public:

    static server_manager& getInstance()
    {
      static server_manager instance;
      return instance;
    }

    server_manager(server_manager const&)  = delete;
    void operator=(server_manager const&)  = delete;

    private:

      server_manager(){};

      void saveConfig(QJsonDocument j_doc);

    public:

      QJsonDocument loadConfig();

      void addServer(const QString &server_uri);
      void removeServer(const QString &server_uri);

      bool getStatusCoverage(const QString &server_uri, const QString &cv_name);
      bool getStatusAttribute(const QString &server_uri, const QString &cv_name, const QString &attribute);

      void changeStatusCoverage(const QString &server_uri, const QString &cv_name);
      void changeStatusAttribute(const QString &server_uri, const QString &cv_name, const QString &attribute);



  };
}

