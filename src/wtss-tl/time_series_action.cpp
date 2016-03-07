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


#include <terralib/qt/af/ApplicationController.h>
#include <terralib/qt/af/events/MapEvents.h>

#include "time_series_action.hpp"
#include "time_series_dialog.hpp"

wtss_tl::time_series_action::time_series_action(QToolBar *toolbar): m_toolBar(toolbar), m_action(0)
{
  createAction(tr("Show Time Series").toStdString(), "chart-time-series");
  m_action->setObjectName("Tools.WTSS.Query");
  m_action->setCheckable(true);
  m_action->setChecked(true);
}


wtss_tl::time_series_action::~time_series_action()
{

}

void wtss_tl::time_series_action::createAction(std::string name, std::string pixmap)
{
  m_action = new QAction(m_toolBar);

  m_action->setText(name.c_str());

  if(pixmap.empty() == false)
    m_action->setIcon(QIcon::fromTheme(pixmap.c_str()));

  connect(m_action, SIGNAL(toggled(bool)), this, SLOT(onActionToggled(bool checked)));

  m_toolBar->addAction(m_action);
}

void wtss_tl::time_series_action::onActionToggled(bool checked)
{
  if(!checked)
    return;

//  te::qt::af::evt::GetMapDisplay e;
//  te::qt::af::AppCtrlSingleton::getInstance().brocast(&e);
//  if(e.m_display)
//  {
//    e.m_display->setCurrentTool();
//  }
}
