// TWS
#include "wtss.hpp"
// TerraLib
#include <terralib/plugin/Plugin.h>

//QT

#include <QObject>

#define TE_QT_WTSS_PLUGIN_NAME "te.qt.wtss"


class QAction;

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


class Plugin : public QObject, public te::plugin::Plugin
{
  Q_OBJECT

  public:

    Plugin(const te::plugin::PluginInfo& pluginInfo);

    ~Plugin();

    void startup();

    void shutdown();

  // protected slots:
  //   void showWindow();

  signals:

    void triggered(te::qt::af::evt::Event* e);

  //protected:
 //   QAction* m_showWindow;

};



#define export_macro

PLUGIN_CALL_BACK_DECLARATION(export_macro)
