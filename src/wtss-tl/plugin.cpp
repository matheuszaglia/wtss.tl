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
 */

//wtss
#include "plugin.hpp"
// STL
#include <memory>
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/events/ApplicationEvents.h>
#include <iostream>

Plugin::Plugin(const te::plugin::PluginInfo& pluginInfo) : 
QObject(),
te::plugin::Plugin(pluginInfo)
{
  te::qt::af::AppCtrlSingleton::getInstance().addListener(this, te::qt::af::SENDER);
}

Plugin::~Plugin()
{
}

void Plugin::startup()
{
  if(m_initialized)
    return;
  //tws::wtss::register_operations();
  //tws::wtss::initialize_operations();

  m_initialized = true;

  {
      // m_showWindow = new QAction(QIcon::fromTheme("file-vector"), tr("Vector File..."), this);
      // m_showWindow->setObjectName("Project.Add Layer.Vector File");

      // te::qt::af::evt::NewActionsAvailable e;
      // e.m_category = "Dataaccess";
      // e.m_actions << m_showWindow;

      // emit triggered(&e);

      // connect (m_showWindow, SIGNAL(triggered()), SLOT(showWindow()));
  }

}

void Plugin::shutdown()
{
  if(!m_initialized)
    return;

  //tws::core::http_server_builder::instance().remove("mongoose");

  m_initialized = false;
}

void Plugin::showWindow()
{
  //place holder
}

PLUGIN_CALL_BACK_IMPL(Plugin)

