

//wtss.tl
#include "time_series_tool.hpp"
#include "time_series_dialog.hpp"
//wtss.cxx
#include <wtss-cxx/wtss.hpp>
//TerraLib
#include <terralib/qt/widgets/canvas/MapDisplay.h>

wtss_tl::time_series_tool::time_series_tool(te::qt::widgets::MapDisplay *display, QObject *parent): te::qt::widgets::AbstractTool(display, parent),
m_dialog(new QDialog(display))
{

}

wtss_tl::time_series_tool::~time_series_tool()
{

}

bool wtss_tl::time_series_tool::mouseReleaseEvent(QMouseEvent *e)
{

  QPointF qtPoint = e->localPos();
  QPointF coordPoint = m_display->transform(qtPoint);

  std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());

  converter->setSourceSRID(m_display->getSRID());
  converter->setTargetSRID(4326);

  te::gm::Coord2D c;

  converter->convert(coordPoint.x(), coordPoint.y(), c.x, c.y);

  QWidget* m_parent = new QWidget(0);
  wtss_cxx::timeseries_query_t query;
  query.longitude = c.x;
  query.latitude  = c.y;

  wtss_tl::time_series_dialog dialog(query,m_parent);


  if(dialog.exec() != QDialog::Accepted)
    return true;

  return true;
}
