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


// Terralib
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/events/ApplicationEvents.h>


//wtss.tl
#include "server_manager.hpp"
#include "server_config_action.hpp"
#include "time_series_action.hpp"

#include "server_config_dialog.hpp"


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

    m_wtssToolBar = new QToolBar;
    m_wtssToolBar->setObjectName("WTSS Toolbar");
    te::qt::af::AppCtrlSingleton::getInstance().addToolBar("WTSSToolbar", m_wtssToolBar);

    m_actionQuery = new QAction(m_wtssToolBar);
    m_actionQuery->setText("Show Time Series");
    m_actionQuery->setIcon(QIcon::fromTheme("sa-spatialanalysis-icon"));
    m_actionQuery->setObjectName("Tools.wtss_query");
    m_actionQuery->setCheckable(true);
    m_wtssToolBar->addAction(m_actionQuery);
    registerActions();
  }

}

void wtss_tl::Plugin::shutdown()
{
  if(!m_initialized)
    return;
  m_initialized = false;  

  te::qt::af::AppCtrlSingleton::getInstance().removeToolBar("WTSSToolbar");
}

void wtss_tl::Plugin::registerActions()
{
  m_serverAction = new QAction(m_wtssMenu);
  m_serverAction->setText("Web Time Series Services...");
  m_serverAction->setIcon(QIcon::fromTheme("chart-time-series"));
  connect(m_serverAction, SIGNAL(triggered()), this, SLOT(onServerActionActivated()));
  m_wtssMenu->addAction(m_serverAction);

  m_timeSeriesAction = new time_series_action(m_wtssToolBar);
  connect(m_timeSeriesAction, SIGNAL(toggled(bool)), this, SLOT(onActionQueryToggled(bool)));
}

void wtss_tl::Plugin::unregisterActions()
{

}

void wtss_tl::Plugin::onServerActionActivated()
{
  QWidget* parent = te::qt::af::AppCtrlSingleton::getInstance().getMainWindow();
  wtss_tl::server_config_dialog dialog(parent);

  if(dialog.exec() != QDialog::Accepted)
    return;

}


PLUGIN_CALL_BACK_IMPL(wtss_tl::Plugin)

