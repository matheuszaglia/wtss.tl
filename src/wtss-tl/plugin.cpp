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
#include "server_manager.hpp"
#include "wtss_dialog.hpp"

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


wtss::tl::Plugin::Plugin(const te::plugin::PluginInfo& pluginInfo)
    : QObject(), te::plugin::CppPlugin(pluginInfo), m_wtssDlg(0)
{
  te::qt::af::AppCtrlSingleton::getInstance().addListener(this,
                                                          te::qt::af::SENDER);
}

wtss::tl::Plugin::~Plugin() {}

void wtss::tl::Plugin::startup()
{
  if(m_initialized)
      return;

  m_initialized = true;

  {
    m_menu = te::qt::af::AppCtrlSingleton::getInstance().getMenu("Plugins");

    m_wtssAction = new QAction(this);
    m_wtssAction->setText("Web Time Series Services");
    m_wtssAction->setIcon(QIcon::fromTheme("chart-time-series"));
    m_wtssAction->setObjectName("Tools.WTSS");

    m_menu->addAction(m_wtssAction);

    m_actionManageServices = new QAction(this);
    m_actionManageServices->setText("Manage Services...");
    m_actionManageServices->setObjectName(
        "Tools.WTSS.Manage Web Time Series Services");
    m_actionManageServices->setIcon(QIcon::fromTheme("preferences-system"));

    m_timeSeriesAction = new QAction(this);
    m_timeSeriesAction->setText("Query Time Series...");
    m_timeSeriesAction->setObjectName("Tools.WTSS.Query Time Series");
    m_timeSeriesAction->setCheckable(true);
    m_timeSeriesAction->setIcon(QIcon::fromTheme("chart-time-series"));
    m_timeSeriesAction->setEnabled(true);

    m_wtssToolBar = new QToolBar("WTSS Toolbar");
    m_wtssToolBar->addAction(m_actionManageServices);
    m_wtssToolBar->addAction(m_timeSeriesAction);
    m_wtssToolBar->setEnabled(true);

    registerActions();

   QActionGroup* m_wtssTool =
        te::qt::af::AppCtrlSingleton::getInstance().findActionGroup(
            "Map.ToolsGroup");
    assert(m_wtssTool);

    m_wtssTool->addAction(m_timeSeriesAction);
  }
}

void wtss::tl::Plugin::shutdown()
{
  if(!m_initialized)
      return;

  m_initialized = false;

  delete m_wtssAction;
  delete m_actionManageServices;
  delete m_timeSeriesAction;
  delete m_wtssToolBar;
  delete m_wtssDlg;

  te::qt::af::AppCtrlSingleton::getInstance().removeToolBar("WTSS Toolbar");

  te::qt::af::AppCtrlSingleton::getInstance().removeListener(this);
}

void wtss::tl::Plugin::registerActions()
{
  connect(m_wtssAction, SIGNAL(triggered(bool)), this,
          SLOT(onActionActivated(bool)));

  connect(m_actionManageServices, SIGNAL(triggered()), this,
          SLOT(onServerActionActivated()));

  connect(m_timeSeriesAction, SIGNAL(toggled(bool)), this,
          SLOT(onActionQueryToggled()));
}

void wtss::tl::Plugin::unregisterActions() {}

te::qt::widgets::MapDisplay* wtss::tl::Plugin::get_map_display()
{
  te::qt::af::BaseApplication* baseApplication =
      dynamic_cast<te::qt::af::BaseApplication*>(
          te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

  return baseApplication->getMapDisplay();
}

void wtss::tl::Plugin::onActionActivated(bool)
{
  te::qt::af::evt::NewActionsAvailable e;
  e.m_toolbar = m_wtssToolBar;
  e.m_category = "WTSSToolbar";

  emit triggered(&e);
}

void wtss::tl::Plugin::onServerActionActivated()
{
  if(!m_wtssDlg)
  {
    m_wtssDlg = new wtss::tl::wtss_dialog(
                te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());

  }

  m_wtssDlg->set_map_display(get_map_display());

  m_wtssDlg->hide_graph(true);

  m_wtssDlg->setModal(false);

  m_wtssDlg->show();
}

void wtss::tl::Plugin::onActionQueryToggled()
{
  if(!m_wtssDlg)
  {
    m_wtssDlg = new wtss::tl::wtss_dialog(
                te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());
  }

  m_wtssDlg->set_map_display(get_map_display());

  m_wtssDlg->onPointPickerToggled(true);

  m_wtssDlg->hide_graph(false);
}

void wtss::tl::Plugin::onClose()
{
  if(m_wtssDlg)
  {
    delete m_wtssDlg;
    m_wtssDlg = 0;
  }
}

PLUGIN_CALL_BACK_IMPL(wtss::tl::Plugin)
