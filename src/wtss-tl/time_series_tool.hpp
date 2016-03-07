

#ifndef __WTSS_TL_TIME_SERIES_TOOL_HPP__
#define __WTSS_TL_TIME_SERIES_TOOL_HPP__

#include <QObject>
#include <QDialog>

#include <terralib/qt/widgets/tools/AbstractTool.h>

namespace wtss_tl
{
  class time_series_tool : public te::qt::widgets::AbstractTool
  {
    Q_OBJECT

    public:

      time_series_tool(te::qt::widgets::MapDisplay* display, QObject* parent = 0);
      ~time_series_tool();

      bool mouseReleaseEvent(QMouseEvent* e);

    private:

      QDialog* m_dialog;

  };
}


#endif // __WTSS_TL_TIME_SERIES_TOOL_HPP__
