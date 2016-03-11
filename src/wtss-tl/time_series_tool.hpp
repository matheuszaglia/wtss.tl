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
  with WTSS.TL. See COPYING. If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/time_series_tool.hpp

  \brief Time series tool for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */



#ifndef __WTSS_TL_TIME_SERIES_TOOL_HPP__
#define __WTSS_TL_TIME_SERIES_TOOL_HPP__


//QT
#include <QObject>
#include <QDialog>

//TerraLib
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
