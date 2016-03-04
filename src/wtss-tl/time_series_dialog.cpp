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
  with TerraLib Web Services. See COPYING. If not, see <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */
//wtss.tl
#include "time_series_dialog.hpp"
#include "ui_time_series_dialog_form.h"
//QT
#include <QInputDialog>
#include <QMessageBox>

#include <iostream>

//wtss.cxx
#include <wtss-cxx/wtss.hpp>

//wtss.tl
#include "server_manager.hpp"

//boost
#include <boost/algorithm/string/join.hpp>

wtss_tl::time_series_dialog::time_series_dialog(QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
m_ui(new Ui::time_series_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Service"));

}


wtss_tl::time_series_dialog::~time_series_dialog()
{

}


