

#ifndef __WTSS_PLUGIN_HPP__
#define __WTSS_PLUGIN_HPP__

#define TE_QT_WTSS_PLUGIN_NAME "te.qt.wtss"

// TerraLib
#include <terralib/plugin/Plugin.h>

//QT
#include <QObject>
#include <QAction>
#include <QMenu>

//STL
#include <vector>

//wtss.tl
#include "server_config.hpp"


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

namespace wtss_tl{
  class server_config_action;
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

      QAction* m_actionManageServers;
      QAction* m_listCoverages;
      QAction* m_describeCoverage;
      QAction* m_timeSeries;
      QMenu* m_wtssMenu;

      server_config_action* m_serverAction;

      std::vector<server_t> servers;
  };

}

#define export_macro

PLUGIN_CALL_BACK_DECLARATION(export_macro)


#endif
