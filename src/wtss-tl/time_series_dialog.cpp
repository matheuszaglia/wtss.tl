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
#include <wtss-cxx/data_types.hpp>
//wtss.tl
#include "time_series_dialog.hpp"
#include "ui_time_series_dialog_form.h"
#include "services_manager.hpp"
//QT
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <QVector>

//boost
#include <boost/algorithm/string/join.hpp>

//QCustomPlot
#include "QCustomPlot.h"

//stl

#include <cstdlib>
#include <ctime>

wtss_tl::time_series_dialog::time_series_dialog(wtss_cxx::timeseries_query_t q, QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
query(q),
m_ui(new Ui::time_series_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Service - Query"));
  loadSettings();

  QApplication::setOverrideCursor(Qt::WaitCursor);
  doQuery();
  QApplication::restoreOverrideCursor();

  plotResult();
}

wtss_tl::time_series_dialog::~time_series_dialog()
{

}

void wtss_tl::time_series_dialog::loadSettings()
{
  QJsonObject j_object = wtss_tl::services_manager::getInstance().loadConfig().object();
  QJsonObject j_server;
  QJsonObject::iterator it;
  for(it = j_object.begin(); it != j_object.end(); ++it)
  {
    j_server = it.value().toObject();
    if(j_server["active"].toBool())
    {
      server_uri = it.key().toStdString();
      break;
    }
  }
  QJsonObject j_coverages = j_server["coverages"].toObject();
  QJsonObject j_coverage;
  for(it = j_coverages.begin(); it != j_coverages.end();it++)
  {
    j_coverage = it.value().toObject();
    if(j_coverage["active"].toBool())
    {
      query.coverage_name = it.key().toStdString();
      break;
    }
  }
  QJsonObject j_attributes = j_coverage["attributes"].toObject();
  QJsonObject j_attribute;
  for(it = j_attributes.begin(); it != j_attributes.end();++it)
  {
    if(it.value().toBool())
      query.attributes.push_back(it.key().toStdString());
  }

}


void wtss_tl::time_series_dialog::doQuery()
{
  wtss_cxx::wtss remote(server_uri);
  result = remote.time_series(query);
}

void wtss_tl::time_series_dialog::plotResult()
{


  m_ui->customPlot->xAxis->setLabel("Timeline");
  m_ui->customPlot->yAxis->setLabel("Values");
  std::vector<wtss_cxx::queried_attribute_t> attributes = result.coverage.queried_attributes;


  QVector<double> timeline(result.coverage.timeline.size());
  for(int i = 0; i < timeline.size(); ++i)
  {
    wtss_cxx::date d = result.coverage.timeline[i];
    QDateTime start = QDateTime(QDate(d.year, d.month, d.day));
    start.setTimeSpec(Qt::UTC);
    timeline[i] = start.toTime_t();
  }

  for(int i = 0;i < attributes.size(); ++i)
  {
    wtss_cxx::queried_attribute_t attribute = attributes[i];
    m_ui->customPlot->addGraph();
    m_ui->customPlot->graph(i)->setName(QString::fromStdString(attribute.name));
    m_ui->customPlot->graph(i)->addData(timeline ,QVector<double>::fromStdVector(attribute.values));
    m_ui->customPlot->graph(i)->setPen(QPen(randomColor()));
  }


  // configure bottom axis to show date and time instead of number:
  m_ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  m_ui->customPlot->xAxis->setDateTimeFormat("MMMM\nyyyy");
  // set a more compact font size for bottom and left axis tick labels:
  m_ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
  m_ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
  // set a fixed tick-step to one tick per month:
  m_ui->customPlot->xAxis->setAutoTickStep(true);
  m_ui->customPlot->xAxis->setTickStep(2628000); // one month in seconds
  m_ui->customPlot->xAxis->setSubTickCount(2000);
  // apply manual tick and tick label for left axis:
  m_ui->customPlot->yAxis->setAutoTicks(false);
  m_ui->customPlot->yAxis->setAutoTickLabels(false);
  m_ui->customPlot->yAxis->setTickVector(QVector<double>() << 5 << 55);
  // set axis labels:
  m_ui->customPlot->xAxis->setLabel("Timeline");
  m_ui->customPlot->yAxis->setLabel("Values");
  // make top and right axes visible but without ticks and labels:
  m_ui->customPlot->xAxis2->setVisible(true);
  m_ui->customPlot->yAxis2->setVisible(true);
  m_ui->customPlot->xAxis2->setTicks(false);
  m_ui->customPlot->yAxis2->setTicks(false);
  m_ui->customPlot->xAxis2->setTickLabels(false);
  m_ui->customPlot->yAxis2->setTickLabels(false);
  // show legend:
  m_ui->customPlot->legend->setVisible(true);

  m_ui->customPlot->rescaleAxes();
  m_ui->customPlot->replot();
}

QColor wtss_tl::time_series_dialog::randomColor()
{
  srand (time(NULL));
  return QColor(rand() % 255, rand() % 255, rand() % 255);
}
