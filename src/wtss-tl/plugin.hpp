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
  \file wtss-tl/plugin.hpp

  \brief Add a plugin interface for dynamic loading of the Web Time Series Data
  Service plugin.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_PLUGIN_HPP__
#define __WTSS_PLUGIN_HPP__

// wtss.tl
#include "config.hpp"
#include "wtss_dialog.hpp"

// TerraLib
#include <terralib/core/plugin/CppPlugin.h>

// QT
#include <QAction>
#include <QMenu>
#include <QObject>
#include <QToolBar>

// STL
#include <vector>
namespace te
{
  namespace qt
  {
    namespace af
    {
      namespace evt
      {
        struct Event;
      }
    }
  }
}

namespace wtss
{
  namespace tl
  {
    class Plugin : public QObject, public te::core::CppPlugin
    {
      Q_OBJECT

     public:
      Plugin(const te::core::PluginInfo& pluginInfo);

      ~Plugin();

      void startup();

      void shutdown();

      te::qt::widgets::MapDisplay* get_map_display();

     protected:
      void registerActions();

      void unregisterActions();

    Q_SIGNALS:

      void triggered(te::qt::af::evt::Event* e);

     protected:
      QMenu* m_menu;
      QAction* m_wtssAction;
      QAction* m_actionManageServices;
      QAction* m_timeSeriesAction;
      QToolBar* m_wtssToolBar;

     protected slots:
      void onServerActionActivated();

      void onActionQueryToggled();

      void onActionActivated(bool);

      void onClose();

     private:
      wtss::tl::wtss_dialog* m_wtssDlg;
    };
  }
}

#endif //__WTSS_PLUGIN_HPP__
