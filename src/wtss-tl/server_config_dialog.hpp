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
  with TerraLib Web Services. See COPYING. If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/server_config_dialog.hpp

  \brief Configuration dialog for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_TL_SERVER_CONFIG_DIALOG_HPP__
#define __WTSS_TL_SERVER_CONFIG_DIALOG_HPP__

// STL
#include <memory>
#include <string>

//QT
#include <QDialog>
#include <QObject>
#include <QtGui>
#include <QListWidgetItem>

//wtss.cxx
#include <wtss-cxx/data_types.hpp>

namespace Ui { class server_config_dialog_form; }

namespace wtss_tl{
  class server_config_dialog : public QDialog
  {
    Q_OBJECT

    public:

      server_config_dialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

      ~server_config_dialog();

    protected slots:

      void onServerAddButtonClicked();

      void onServerRemoveButtonClicked();

      void onListServerItemSelected();

      void onListCoverageChecked(QListWidgetItem *item);

    private:

      std::auto_ptr<Ui::server_config_dialog_form> m_ui;
      QString server_uri;
      QJsonObject j_config;
  };
}

#endif //__WTSS_TL_SERVER_CONFIG_DIALOG_HPP__
