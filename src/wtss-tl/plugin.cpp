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
  with TerraLib Web Services. See COPYING. If not, see
  <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/plugin.cpp

  \brief Add a plugin interface for dynamic loading of the Web Time Series Data
  Service plugin.

  \author Matheus Cavassan Zaglia
 */

// wtss
#include "plugin.hpp"

// QT
#include <QApplication>

// Terralib
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/Utils.h>
#include <terralib/qt/af/events/ApplicationEvents.h>
#include <terralib/qt/af/events/MapEvents.h>

#include <terralib/qt/af/BaseApplication.h>

#include <terralib/qt/widgets/canvas/MapDisplay.h>
#include <terraview/TerraView.h>

// wtss.tl
#include "server_manager.hpp"
#include "wtss_dialog.hpp"
#include "wtss_tool.hpp"

wtss::tl::Plugin::Plugin(const te::plugin::PluginInfo& pluginInfo)
    : QObject(), te::plugin::Plugin(pluginInfo), m_wtssDlg(0)
{
  te::qt::af::AppCtrlSingleton::getInstance().addListener(this,
                                                          te::qt::af::SENDER);
}

wtss::tl::Plugin::~Plugin() {}
void wtss::tl::Plugin::startup()
{
  if(m_initialized) return;

  m_initialized = true;

  {
    m_menu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("Tools");
    QAction* m_act = te::qt::af::AppCtrlSingleton::getInstance().findAction(
        "Tools.Customize");

    m_wtssMenu = new QMenu(m_menu);
    m_wtssMenu->setTitle("Web Time Series Services");
    m_wtssMenu->setIcon(QIcon::fromTheme("chart-time-series"));
    m_wtssMenu->setObjectName("Tools.WTSS");

    QAction* pluginsSeparator =
        te::qt::af::AppCtrlSingleton::getInstance().findAction(
            "ManagePluginsSeparator");
    m_menu->insertMenu(m_act, m_wtssMenu);
    m_menu->insertAction(m_act, pluginsSeparator);

    m_actionManageServices = new QAction(m_wtssMenu);
    m_actionManageServices->setText("Manage Services...");
    m_actionManageServices->setObjectName(
        "Tools.WTSS.Manage Web Time Series Services");
    m_actionManageServices->setIcon(QIcon::fromTheme("preferences-system"));
    m_wtssMenu->addAction(m_actionManageServices);

    m_timeSeriesAction = new QAction(m_wtssMenu);
    m_timeSeriesAction->setText("Query Time Series...");
    m_timeSeriesAction->setObjectName("Tools.WTSS.Query Time Series");
    m_timeSeriesAction->setCheckable(true);
    m_timeSeriesAction->setIcon(QIcon::fromTheme("chart-time-series"));
    m_timeSeriesAction->setEnabled(true);
    m_wtssMenu->addAction(m_timeSeriesAction);

    m_wtssToolBar =
        te::qt::af::AppCtrlSingleton::getInstance().getToolBar("WTSSToolBar");

    if(m_wtssToolBar == 0)
    {
      m_wtssToolBar = new QToolBar;
      m_wtssToolBar->setObjectName("WTSS Tool Bar");
      m_wtssToolBar->addAction(m_timeSeriesAction);
      m_wtssToolBar->setEnabled(true);
    }

    te::qt::af::AppCtrlSingleton::getInstance().addToolBar("WTSSToolBar",
                                                           m_wtssToolBar);

    registerActions();

    te::qt::af::AddToolBarToSettings(m_wtssToolBar);
    te::qt::af::UpdateToolBarsInTheSettings(
        &te::qt::af::AppCtrlSingleton::getInstance());

    QActionGroup* m_wtssTool =
        te::qt::af::AppCtrlSingleton::getInstance().findActionGroup(
            "Map.ToolsGroup");
    assert(m_wtssTool);

    m_wtssTool->addAction(m_timeSeriesAction);
  }
}

void wtss::tl::Plugin::shutdown()
{
  if(!m_initialized) return;
  m_initialized = false;

  delete m_actionManageServices;
  delete m_timeSeriesAction;
  //  delete m_actionQuery;
  te::qt::af::RemoveToolBarFromSettings(m_wtssToolBar);
  te::qt::af::UpdateToolBarsInTheSettings(
      &te::qt::af::AppCtrlSingleton::getInstance());
  delete m_wtssToolBar;
  delete m_wtssMenu;
  te::qt::af::AppCtrlSingleton::getInstance().removeToolBar("WTSSToolBar");
}

void wtss::tl::Plugin::registerActions()
{
  connect(m_actionManageServices, SIGNAL(triggered()), this,
          SLOT(onServerActionActivated()));
  connect(m_timeSeriesAction, SIGNAL(toggled(bool)), this,
          SLOT(onActionQueryToggled()));
}

void wtss::tl::Plugin::unregisterActions() {}

void wtss::tl::Plugin::onServerActionActivated()
{
  m_wtssDlg = new wtss::tl::wtss_dialog(
      te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

  te::qt::af::BaseApplication* baseApplication =
      dynamic_cast<te::qt::af::BaseApplication*>(
          te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

  m_wtssDlg->set_map_display(baseApplication->getMapDisplay());

  m_wtssDlg->setModal(false);

  m_wtssDlg->show();
}

void wtss::tl::Plugin::onActionQueryToggled()
{
  te::qt::af::BaseApplication* baseApplication =
      dynamic_cast<te::qt::af::BaseApplication*>(
          te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

  if(baseApplication->getMapDisplay())
  {
    baseApplication->getMapDisplay()->setCursor(Qt::CrossCursor);
    wtss::tl::time_series_tool* tool =
        new wtss::tl::time_series_tool(baseApplication->getMapDisplay());
    baseApplication->getMapDisplay()->setCurrentTool(tool);
  }
}

PLUGIN_CALL_BACK_IMPL(wtss::tl::Plugin)
