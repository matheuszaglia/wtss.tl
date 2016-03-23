/*
  Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.

  This file is part of the WTSS.CXX.

  WTSS.TL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 3 as
  published by the Free Software Foundation.

  WTSS.TL is distributed  "AS-IS" in the hope that it will be useful,
  but WITHOUT ANY WARRANTY OF ANY KIND; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along
  with WTSS.TL. See COPYING. If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file unittest/main.cpp

  \brief Unit-test for WTSS.TL.

  \author Matheus Cavassan Zaglia
 */

// WTSS.TL
#include <wtss-cxx/wtss.hpp>
#include "time_series_dialog.hpp"

//TerraLib
#include <terralib/common.h>
#include <terralib/datatype/Date.h>


#include <terralib/st/core/timeseries/TimeSeries.h>
#include <terralib/qt/widgets/charts/TimeSeriesChart.h>
#include <terralib/qt/widgets/charts/ChartDisplay.h>
#include <terralib/qt/widgets/charts/ChartDisplayWidget.h>
#include <terralib/qt/widgets/charts/ChartStyle.h>

// STL
#include <cstdlib>

#include <QApplication>


void generateTimeSeries()
{
  wtss_cxx::wtss remote("http://www.dpi.inpe.br/mds");
  std::vector<std::string> coverages = remote.list_coverages();
  te::st::TimeSeries* ts = new te::st::TimeSeries(coverages.front());

  wtss_cxx::geoarray_t cv = remote.describe_coverage(coverages.front());

  wtss_cxx::timeseries_query_t q;
  q.coverage_name = coverages.front();
  q.attributes.push_back(cv.attributes.front().name);
  q.longitude = -54.0;
  q.latitude = -12;

  wtss_cxx::timeseries_query_result_t result = remote.time_series(q);
  std::vector<wtss_cxx::queried_attribute_t> attributes = result.coverage.queried_attributes;

  for(unsigned int i = 0;i < attributes.size(); i++)
  {
    wtss_cxx::queried_attribute_t attribute = attributes[i];
    for(unsigned int j = 0; j < attribute.values.size(); ++j)
    {
    wtss_cxx::date d = result.coverage.timeline[j];
    te::dt::DateTime* time = new te::dt::Date(boost::gregorian::date(d.year,d.month, d.day));
    double value = attribute.values[j];
    ts->add(time, value);
    }
  }

  te::qt::widgets::TimeSeriesChart* timeseries = new te::qt::widgets::TimeSeriesChart(ts);


  te::qt::widgets::ChartStyle* chartStyle = new te::qt::widgets::ChartStyle();
  chartStyle->setTitle("Time Series");
  chartStyle->setAxisX("time");
  chartStyle->setAxisY("value");

  te::qt::widgets::ChartDisplay* chartDisplay = new te::qt::widgets::ChartDisplay(0, "Time Series", chartStyle);
  chartDisplay->adjustDisplay();
  timeseries->attach(chartDisplay);

  te::qt::widgets::ChartDisplayWidget* displayWidget = new te::qt::widgets::ChartDisplayWidget(timeseries, te::qt::widgets::TIMESERIES_CHART, chartDisplay);
  displayWidget->show();
  displayWidget->setWindowTitle("Time Series");
  displayWidget->setAttribute(Qt::WA_DeleteOnClose, true);

}

int main(int argc, char* argv[])
{


  QApplication app(argc, argv);

  generateTimeSeries();

  int ret = app.exec();
  return ret;

}
