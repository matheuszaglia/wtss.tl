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


#include <terralib/qt/af/ApplicationController.h>

#include "server_config_action.hpp"
#include "server_config_dialog.hpp"

wtss_tl::server_config_action::server_config_action(QMenu *menu) : wtss_tl::abstract_action(menu)
{
  createAction(tr("Web Time Series Servers...").toStdString(), "chart-time-series");
  m_action->setObjectName("Tools.WTSS.Web Time Series Server");
}


wtss_tl::server_config_action::~server_config_action()
{

}

void wtss_tl::server_config_action::onActionActivated()
{
  QWidget* parent = te::qt::af::AppCtrlSingleton::getInstance().getMainWindow();
  wtss_tl::server_config_dialog dialog(parent);

  if(dialog.exec() != QDialog::Accepted)
    return;

}
