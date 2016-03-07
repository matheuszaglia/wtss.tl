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
#include <terralib/qt/af/events/MapEvents.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>

//wtss.tl
#include "server_manager.hpp"
#include "server_config_dialog.hpp"
#include "time_series_tool.hpp"


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

    m_menu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("Tools");
    m_wtssMenu = new QMenu(m_menu);
    m_wtssMenu->setTitle("Web Time Series Services");
    m_wtssMenu->setIcon(QIcon::fromTheme("chart-time-series"));
    m_wtssMenu->setObjectName("Tools.WTSS");

    QAction* pluginsSeparator = te::qt::af::AppCtrlSingleton::getInstance().findAction("ManagePluginsSeparator");
    m_menu->insertMenu(pluginsSeparator, m_wtssMenu);

    m_actionManageServices = new QAction(m_wtssMenu);
    m_actionManageServices->setText("Manage Services...");
    m_actionManageServices->setObjectName("Tools.WTSS.ManageServices");
    m_actionManageServices->setIcon(QIcon::fromTheme("view-datasource-explorer"));
    m_wtssMenu->addAction(m_actionManageServices);

    m_timeSeriesAction = new QAction(m_wtssMenu);
    m_timeSeriesAction->setText("Query Time Series...");
    m_timeSeriesAction->setObjectName("Tools.WTSS.Query");
    m_timeSeriesAction->setCheckable(true);
    m_timeSeriesAction->setIcon(QIcon::fromTheme("chart-time-series"));

    m_wtssToolBar = new QToolBar;
    m_wtssToolBar->setObjectName("WTSS Toolbar");
    te::qt::af::AppCtrlSingleton::getInstance().addToolBar("WTSSToolbar", m_wtssToolBar);
    m_wtssToolBar->addAction(m_actionManageServices);
    m_wtssToolBar->addAction(m_timeSeriesAction);

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

  connect(m_actionManageServices, SIGNAL(triggered()), this, SLOT(onServerActionActivated()));
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


void wtss_tl::Plugin::onActionQueryToggled(bool checked)
{
  if(!checked)
    return;

  te::qt::af::evt::GetMapDisplay e;
  if (e.m_display)
  {
    wtss_tl::time_series_tool* tool = new wtss_tl::time_series_tool(e.m_display->getDisplay());
    e.m_display->setCurrentTool(tool);
  }
}


PLUGIN_CALL_BACK_IMPL(wtss_tl::Plugin)

