

#ifndef __WTSS_PLUGIN_HPP__
#define __WTSS_PLUGIN_HPP__

#define TE_QT_WTSS_PLUGIN_NAME "te.qt.wtss"

// TerraLib
#include <terralib/plugin/Plugin.h>

//QT
#include <QObject>
#include <QAction>
#include <QMenu>
#include <QToolBar>

//STL
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

namespace wtss_tl{

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
      void onActionQueryToggled(bool checked);
  };

}

#define export_macro

PLUGIN_CALL_BACK_DECLARATION(export_macro)


#endif
