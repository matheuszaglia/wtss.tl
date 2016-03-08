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
  \file wtss-tl/time_series_dialog.cpp

  \brief Time series query dialog for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

//wtss.cxx
#include <wtss-cxx/wtss.hpp>


//wtss.tl
#include "time_series_dialog.hpp"
#include "ui_time_series_dialog_form.h"
#include "services_manager.hpp"

//QT
#include <QInputDialog>
#include <QMessageBox>
#include <QString>

//boost
#include <boost/algorithm/string/join.hpp>

wtss_tl::time_series_dialog::time_series_dialog(wtss_cxx::timeseries_query_t query, QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
query(query),
m_ui(new Ui::time_series_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Service - Query"));
  m_ui->txtLongitude->setText(QString::number(query.longitude));
  m_ui->txtLatitude->setText(QString::number(query.latitude));
}


wtss_tl::time_series_dialog::~time_series_dialog()
{

}


