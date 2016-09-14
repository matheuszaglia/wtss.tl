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
  with WTSS.TL. See COPYING. If not, see
  <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/server_manager.hpp

  \brief Manage the JSON file for services configuration of the Web Time Series
  Services plugin.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_TL_SERVER_MANAGER_HPP__
#define __WTSS_TL_SERVER_MANAGER_HPP__

// STL
#include <string>

// QT
#include <QJsonDocument>

namespace wtss
{
  namespace tl
  {
    class server_manager
    {
     public:
      static server_manager &getInstance()
      {
        static server_manager instance;
        return instance;
      }

     public:
      QJsonDocument loadSettings();

      void addServer(const QString &server_uri);
      void removeServer(const QString &server_uri);
      void refreshServer(const QString &server_uri);

      QJsonObject getAttribute(const QString &server_uri,
                               const QString &cv_name,
                               const QString &attribute);

      void changeStatusServer(const QString &server_uri);

      void changeStatusCoverage(const QString &server_uri,
                                const QString &cv_name);

      void changeStatusAttribute(const QString &server_uri,
                                 const QString &cv_name,
                                 const QString &attribute);

     private:
      server_manager() {}
      ~server_manager() {}
      // no copy allowed
      server_manager(server_manager const &) = delete;
      server_manager &operator=(server_manager const &) = delete;

      void saveSettings(QJsonDocument j_doc);
    };
  }  // end namespace tl
}  // end namespace wtss
#endif  // __WTSS_TL_SERVER_MANAGER_HPP__
