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

#define TE_QT_WTSS_PLUGIN_NAME "te.qt.wtss"

#include "wtss-tl/wtss_dialog.hpp"

// TerraLib
#include <terralib/plugin/Plugin.h>

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
    class Plugin : public QObject, public te::plugin::Plugin
    {
      Q_OBJECT

     public:
      Plugin(const te::plugin::PluginInfo& pluginInfo);

      ~Plugin();

      void startup();

      void shutdown();

     protected:
      void registerActions();

      void unregisterActions();

    Q_SIGNALS:

      void triggered(te::qt::af::evt::Event* e);

     protected:
      QAction* m_actionManageServices;
      QAction* m_actionQuery;
      QMenu* m_menu;
      QMenu* m_wtssMenu;
      QToolBar* m_wtssToolBar;
      QAction* m_timeSeriesAction;

     protected slots:
      void onServerActionActivated();
      void onActionQueryToggled();

     private:
      wtss::tl::wtss_dialog* m_wtssDlg;
    };
  }
}

#define export_macro

PLUGIN_CALL_BACK_DECLARATION(export_macro)

#endif
