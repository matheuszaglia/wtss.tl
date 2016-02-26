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
  \file wtss-tl/wtss/plugin.cpp

  \brief Add a plugin interface for dynamic loading of the Web Time Series Data Service plugin.

  \author Gilberto Ribeiro de Queiroz
  \author Matheus Cavassan Zaglia
 */

//wtss
#include "plugin.hpp"
// STL
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/events/ApplicationEvents.h>
#include <iostream>

//wtss.tl
#include "wtss.hpp"
#include "server_config_action.hpp"


wtss_tl::Plugin::Plugin(const te::plugin::PluginInfo& pluginInfo) :
  QObject(),
  te::plugin::Plugin(pluginInfo)
{
  te::qt::af::AppCtrlSingleton::getInstance().addListener(this, te::qt::af::SENDER);
}

wtss_tl::Plugin::~Plugin()
{
}

void wtss_tl::Plugin::startup()
{
  if(m_initialized)
    return;

  m_initialized = true;

  {

    m_wtssMenu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("Tools");

//    m_wtssMenu = new QMenu(m_menu);
//    m_actionManageServers->setText("Web Time Series Servers...");
//    m_actionManageServers->setIcon(QIcon::fromTheme("chart-time-series"));
//    m_actionManageServers->setObjectName("WTSS.manage");

    registerActions();

  }

}

void wtss_tl::Plugin::shutdown()
{
  if(!m_initialized)
    return;

  m_initialized = false;
}

void wtss_tl::Plugin::registerActions()
{
  m_serverAction = new wtss_tl::server_config_action(m_wtssMenu);
  connect(m_serverAction, SIGNAL(triggered()), SLOT(onActionActivated()));
}

void wtss_tl::Plugin::unregisterActions()
{

}

PLUGIN_CALL_BACK_IMPL(wtss_tl::Plugin)

