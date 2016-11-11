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
  with WTSS.TL. See COPYING. If not, see
  <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/wtss_tool.cpp

  \brief Time series tool for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

// wtss.tl
#include "wtss_tool.hpp"
#include "wtss_dialog.hpp"

// wtss.cxx
#include <wtss-cxx/wtss.hpp>

// TerraLib
#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/widgets/canvas/MapDisplay.h>

wtss::tl::wtss_tool::wtss_tool(
    te::qt::widgets::MapDisplay *display, QObject *parent)
    : te::qt::widgets::AbstractTool(display, parent),
      m_wtssDlg(0)
{
}

wtss::tl::wtss_tool::~wtss_tool()
{
  if(m_wtssDlg)
  {
    delete m_wtssDlg;
    m_wtssDlg = 0;
  }
}

bool wtss::tl::wtss_tool::mouseReleaseEvent(QMouseEvent *e)
{  
  QPointF qtPoint = e->localPos();
  QPointF coordPoint = m_display->transform(qtPoint);

  if(e->button() != Qt::LeftButton)
    return false;

  std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());

  converter->setSourceSRID(m_display->getSRID());
  converter->setTargetSRID(4326);

  te::gm::Coord2D c;

  converter->convert(coordPoint.x(), coordPoint.y(), c.x, c.y);

  wtss::cxx::timeseries_query_t query;
  query.longitude = c.x;
  query.latitude = c.y;

  if(!m_wtssDlg)
  {
    m_wtssDlg = new wtss::tl::wtss_dialog(
        te::qt::af::AppCtrlSingleton::getInstance().getMainWindow());
  }

  m_wtssDlg->do_timeseries_query(query);

  m_wtssDlg->setModal(false);

  m_wtssDlg->show();

  return true;
}
