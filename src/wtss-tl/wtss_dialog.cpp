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
  with TerraLib Web Services. See COPYING. If not, see
  <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/wtss_dialog.cpp

  \brief Configuration dialog for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

// wtss.tl
#include "wtss_dialog.hpp"
#include "exception.hpp"
#include "server_manager.hpp"
#include "ui_wtss_dialog_form.h"

// QT
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

// Boost
#include <boost/format.hpp>

// TerraLib
#include <terralib/qt/widgets/utils/ScopedCursor.h>
#include <terralib/qt/widgets/tools/AbstractTool.h>
#include <terralib/maptools/MarkRendererManager.h>
#include <terralib/se/Fill.h>
#include <terralib/se/Mark.h>
#include <terralib/se/Stroke.h>
#include <terralib/se/Utils.h>
#include <terralib/qt/widgets/tools/PointPicker.h>

// STL
#include <ctime>
#include <fstream>

// wtss.cxx
#include <wtss-cxx/wtss.hpp>

wtss::tl::wtss_dialog::wtss_dialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      m_ui(new Ui::wtss_dialog_form),
      dirty(false),
      m_checkServer(0),
      m_checkCoverage(0),
      m_checkAttribute(0),
      m_mapDisplay(0),
      m_chartDisplay(0),
      m_chartStyle(0),
      m_timeSeriesChart(0),
      m_timeSeriesVec(0)
{
  m_ui->setupUi(this);

  this->setWindowTitle(tr("Web Time Series Services"));

  m_ui->btnAddServer->setIcon(QIcon::fromTheme("list-add"));
  m_ui->btnRemoveServer->setIcon(QIcon::fromTheme("list-remove"));
  m_ui->btnRefreshServer->setIcon(QIcon::fromTheme("view-refresh"));
  m_ui->m_hideToolButton->setIcon(QIcon("share/wtss-tl/icons/hide.svg"));
  m_ui->m_hideCoordSToolButton->setIcon(QIcon("share/wtss-tl/icons/hide.svg"));

  define_display();

  define_marker();

  srand(time(NULL));

  connect(m_ui->btnAddServer, SIGNAL(clicked()), this,
          SLOT(onServerAddButtonClicked()));
  connect(m_ui->btnRemoveServer, SIGNAL(clicked()), this,
          SLOT(onServerRemoveButtonClicked()));
  connect(m_ui->m_serverTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
          this, SLOT(onItemChecked(QTreeWidgetItem *)));
  connect(m_ui->btnHelp, SIGNAL(clicked()), this, SLOT(onHelpButtonClicked()));
  connect(m_ui->btnClose, SIGNAL(clicked()), this,
          SLOT(close()));
  connect(m_ui->m_hideToolButton, SIGNAL(clicked()), this,
          SLOT(onHideButtonClicked()));
  connect(m_ui->btnRefreshServer, SIGNAL(clicked()), this,
          SLOT(onServerRefreshButtonClicked()));
  connect(m_ui->btnImport, SIGNAL(clicked()), this,
          SLOT(onImportGraphClicked()));
  connect(m_ui->btnExport, SIGNAL(clicked()), this,
          SLOT(onExportGraphClicked()));
  connect(m_ui->btnQuery, SIGNAL(clicked()), this,
          SLOT(onQueryButtonClicked()));
  connect(m_ui->m_hideCoordSToolButton, SIGNAL(clicked()), this,
          SLOT(onHideCoordSelectedsClicked()));
  connect(m_ui->m_coordSListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(onAddCoordToList(QListWidgetItem*)));

//  connect(m_chartDisplay->axisWidget(QwtPlot::xBottom), SIGNAL(scaleDivChanged()), this,
//          SLOT(onUpdateZoom()));

  load_settings();

  dirty = true;
}

wtss::tl::wtss_dialog::~wtss_dialog() {}

void wtss::tl::wtss_dialog::set_map_display(
    te::qt::widgets::MapDisplay *mapDisplay)
{
  m_mapDisplay = mapDisplay;
}

void wtss::tl::wtss_dialog::hide_graph(bool check)
{
  if(check)
  {
    m_ui->graphFrame->hide();
    m_ui->m_hideToolButton->hide();
    m_ui->coordSframe->hide();
    m_ui->m_hideCoordSToolButton->hide();
    wtss_dialog::adjustSize();
  }
  else
  {
   m_ui->graphFrame->show();
   m_ui->m_hideToolButton->show();
   m_ui->coordSframe->show();
   m_ui->m_hideCoordSToolButton->show();
  }
}

void wtss::tl::wtss_dialog::onPointPickerToggled(bool checked)
{
  if(!checked)
    return;

  te::qt::widgets::PointPicker* point = new te::qt::widgets::PointPicker(m_mapDisplay, Qt::CrossCursor);
  m_mapDisplay->setCurrentTool(point);

  connect(point, SIGNAL(pointPicked(QPointF&)), this, SLOT(onGetPointCoordinate(QPointF&)));
}

void wtss::tl::wtss_dialog::onServerAddButtonClicked()
{
  dirty = false;

  QInputDialog *inputDialog = new QInputDialog();
  inputDialog->setOptions(QInputDialog::NoButtons);

  bool accept;

  QString uri = inputDialog->getText(NULL, "Add Server", "Server URI:",
                                     QLineEdit::Normal, "", &accept);
  if(!accept)
  {
    dirty = true;
    return;
  }

  if(!uri.isEmpty())
  {
     te::qt::widgets::ScopedCursor c(Qt::WaitCursor);

     try
     {
        wtss::tl::server_manager::getInstance().addServer(uri);

        j_config =
            wtss::tl::server_manager::getInstance().loadSettings().object();

        add_server(uri);
     }
     catch(...)
     {
        QMessageBox::warning(this, tr("Web Time Series"), tr("An error has occurred, "
                                                     "please retype the wtss "
                                                     "server address."));
     }
  }
  else
  {
    QMessageBox::warning(this, tr("Web Time Series"), tr("Please, input a server."));
  }

  dirty = true;
}

void wtss::tl::wtss_dialog::onServerRemoveButtonClicked()
{
  dirty = false;

  if(m_ui->m_serverTreeWidget->currentItem() &&
     !m_ui->m_serverTreeWidget->currentItem()->parent())
  {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Remove server",
        "Remove '" + m_ui->m_serverTreeWidget->currentItem()->text(0) +
            "' server?",
        QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
       try
       {
          wtss::tl::server_manager::getInstance().removeServer(
                m_ui->m_serverTreeWidget->currentItem()->text(0));

          j_config =
              wtss::tl::server_manager::getInstance().loadSettings().object();

          delete m_ui->m_serverTreeWidget->currentItem();

          m_chartDisplay->detachItems(te::qt::widgets::TIMESERIES_CHART);

          QMessageBox::information(this, tr("Web Time Series"),
                                   tr("The server was removed with success."));
       }
       catch(...)
       {
          QMessageBox::warning(this, tr("Web Time Series"),
                               tr("An error has occurred, please try again."));
       }
    }
  }
  else
  {
     QMessageBox::warning(this, tr("Web Time Series"),
                          tr("Please, select a server."));
  }

  dirty = true;
}

void wtss::tl::wtss_dialog::onServerRefreshButtonClicked()
{
  dirty = false;
  if(m_ui->m_serverTreeWidget->currentItem() &&
     !m_ui->m_serverTreeWidget->currentItem()->parent())
  {
    try
    {
       QString uri = m_ui->m_serverTreeWidget->currentItem()->text(0);

       wtss::tl::server_manager::getInstance().refreshServer(uri);

       delete m_ui->m_serverTreeWidget->currentItem();

       add_server(uri);
    }
    catch(...)
    {
       QMessageBox::warning(this, tr("Web Time Series"),
                            tr("An error has occurred, "
                               "please check server list."));
    }
  }
  else
  {
     QMessageBox::warning(this, tr("Web Time Series"),
                          tr("Please, select a server."));
  }

  dirty = true;
}

void wtss::tl::wtss_dialog::onItemChecked(QTreeWidgetItem *item)
{
  if(dirty)
  {
    if(item->type() == tree_item_type::attribute)
    {
      QString server_uri = item->parent()->parent()->text(0);
      QString cv_name = item->parent()->text(0);
      QString attribute = item->text(0);

      wtss::tl::server_manager::getInstance().changeStatusAttribute(
          server_uri, cv_name, attribute);
    }
    else if(item->type() == tree_item_type::coverage)
    {
      QString server_uri = item->parent()->text(0);
      QString cv_name = item->text(0);
      wtss::tl::server_manager::getInstance().changeStatusCoverage(server_uri,
                                                                   cv_name);
      dirty = false;
      for(int i = 0; i < item->parent()->childCount(); ++i)
      {
        QTreeWidgetItem *cv = item->parent()->child(i);
        if(cv->text(0) != cv_name)
        {
          if(cv->checkState(0)) cv->setCheckState(0, Qt::Unchecked);
        }
      }
      dirty = true;
    }
    else if(item->type() == tree_item_type::server)
    {
      QString server_uri = item->text(0);

      wtss::tl::server_manager::getInstance().changeStatusServer(server_uri);
      dirty = false;
      int size = m_ui->m_serverTreeWidget->topLevelItemCount();
      for(int i = 0; i < size; ++i)
      {
        QTreeWidgetItem *sv = m_ui->m_serverTreeWidget->topLevelItem(i);
        QString teste = sv->text(0);
        if(teste != server_uri) sv->setCheckState(0, Qt::Unchecked);
      }
      dirty = true;
    }
  }
}

void wtss::tl::wtss_dialog::onHideButtonClicked()
{
  if(m_ui->serverFrame->isVisible())
  {
    m_ui->serverFrame->hide();
    m_ui->m_hideToolButton->setIcon(QIcon("share/wtss-tl/icons/show.svg"));
  }
  else
  {
    m_ui->serverFrame->show();
    m_ui->m_hideToolButton->setIcon(QIcon("share/wtss-tl/icons/hide.svg"));
  }
}

void wtss::tl::wtss_dialog::onHideCoordSelectedsClicked()
{
  if(m_ui->coordSframe->isVisible())
  {
    m_ui->coordSframe->hide();
    m_ui->m_hideCoordSToolButton->setIcon(QIcon("share/wtss-tl/icons/show.svg"));
  }
  else
  {
    m_ui->coordSframe->show();
    m_ui->m_hideCoordSToolButton->setIcon(QIcon("share/wtss-tl/icons/hide.svg"));
  }
}

void wtss::tl::wtss_dialog::onHelpButtonClicked() {}

void wtss::tl::wtss_dialog::onCloseButtonClicked()
{
  clear_canvas();
  this->close();
}

void wtss::tl::wtss_dialog::onImportGraphClicked()
{
  QString csvFile =
      QFileDialog::getOpenFileName(this, "Select a TimeSeries CSV file",
                                   QDir::currentPath(), "CSV file (*.csv)");

  if(csvFile.isEmpty())
    return;

  QFileInfo file(csvFile);
  std::ifstream csv(csvFile.toUtf8().data());

  std::string str;
  std::getline(csv, str);

  m_timeSeriesVec.clear();

  m_timeSeriesVec.push_back(new te::st::TimeSeries(file.baseName().toUtf8().data()));

  while(std::getline(csv, str))
  {
    QString qstr = QString::fromUtf8(str.c_str());
    QStringList fields = qstr.split(',');
    QStringList date = fields.front().split("/");

    te::dt::DateTime* time =
        new te::dt::Date(boost::gregorian::date(date.at(2).toInt(),
                                                date.at(1).toInt(),
                                                date.at(0).toInt()));

    double value = fields.back().toDouble();

    m_timeSeriesVec.back()->add(time, value);
  }
  plot_time_series();
}

void wtss::tl::wtss_dialog::onExportGraphClicked()
{
  if(m_result.query.attributes.empty())
     return;

  QString cv_name(QString::fromUtf8(m_result.coverage.name.c_str()));

  std::vector<wtss::cxx::queried_attribute_t> attributes =
      m_result.coverage.queried_attributes;

  for(unsigned int i = 0; i < attributes.size(); i++)
  {
     wtss::cxx::queried_attribute_t attribute = attributes[i];

     QString csvFile = QFileDialog::getSaveFileName(
                 this, tr("Save File"),
                 QDir::currentPath() + "/" + attribute.name.c_str() + ".csv",
                 "CSV file (*.csv)");

     if(csvFile.isEmpty())
       return;

     QFileInfo info(csvFile);

     if(info.suffix().isEmpty())
       csvFile.append(".csv");

     std::ofstream myfile(csvFile.toUtf8().data());

     myfile << "Timeline,Value" << std::endl;

     for(unsigned int j = 0; j < attribute.values.size(); ++j)
     {
       QJsonObject j_attribute =
           wtss::tl::server_manager::getInstance().getAttribute(
               QString::fromUtf8(m_lastQueriedServer.c_str()), cv_name, QString::fromUtf8(attribute.name.c_str()));

        wtss::cxx::date d = m_result.coverage.timeline[j];

        double value = attribute.values[j] * j_attribute["scale_factor"].toDouble();

        myfile << d.day << "/" << d.month << "/" << d.year
               << "," << value << std::endl;
     }
     myfile.close();
  }
}

void wtss::tl::wtss_dialog::onQueryButtonClicked()
{
  wtss::cxx::timeseries_query_t query;

  if(m_ui->latLineEdit->text().isEmpty() &&
     m_ui->longLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Web Time Series"),
                         tr("Input the latitude and longitude coordinate."));
    return;
  }
  else if(m_ui->latLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Web Time Series"),
                         tr("Input the latitude coordinate."));
    return;
  }
  else if(m_ui->longLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Web Time Series"),
                         tr("Input the longitude coordinate."));
    return;
  }

  query.latitude = m_ui->latLineEdit->text().toDouble();
  query.longitude = m_ui->longLineEdit->text().toDouble();

  QDate startDate = m_ui->m_startDateEdit->date();
  QDate endDate = m_ui->m_endDateEdit->date();

  if(!startDate.operator <=(endDate))
  {
    QMessageBox::warning(this, tr("Web Time Series"),
                         tr("The date informed is invalid."));
    return;
  }

  wtss::tl::server_manager::getInstance().addDateFilter(startDate.toString("dd/MM/yyyy"),
                                                        endDate.toString("dd/MM/yyyy"));

  do_timeseries_query(query);
}

void wtss::tl::wtss_dialog::onAddCoordToList(QListWidgetItem* coordSelected)
{
  wtss::cxx::timeseries_query_t query;

  QStringList coord = coordSelected->text().split(",");

  query.longitude = coord.front().toDouble();
  query.latitude = coord.back().toDouble();

  do_timeseries_query(query);

  add_marker(query.longitude, query.latitude);
}

void wtss::tl::wtss_dialog::onGetPointCoordinate(QPointF& coord)
{
  std::auto_ptr<te::srs::Converter> converter(new te::srs::Converter());

  converter->setSourceSRID(m_mapDisplay->getSRID());
  converter->setTargetSRID(4326);

  te::gm::Coord2D c;

  converter->convert(coord.x(), coord.y(), c.x, c.y);

  wtss::cxx::timeseries_query_t query;

  query.longitude = c.x;
  query.latitude = c.y;

  if(m_ui->m_serverTreeWidget->topLevelItemCount() > 0)
  {
     do_timeseries_query(query);

     add_marker(query.longitude, query.latitude);
  }

  wtss_dialog::setModal(false);
  wtss_dialog::show();
}

void wtss::tl::wtss_dialog::onUpdateZoom()
{
//  QwtScaleDiv scaleDiv = m_chartDisplay->axisScaleDiv(QwtPlot::xBottom);

//  if(scaleDiv.isEmpty())
//      return;

//  if(scaleDiv.lowerBound() < m_lowerBound)
//  {
//     scaleDiv.setLowerBound(m_lowerBound);
//     scaleDiv.setUpperBound(m_lowerBound + scaleDiv.range());

//     if(scaleDiv.upperBound() > m_upperBound ||
//             qFuzzyCompare(scaleDiv.range(), m_upperBound - m_lowerBound))
//         scaleDiv.setUpperBound(m_upperBound);

//     m_chartDisplay->setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);
//  }
//  else if(scaleDiv.upperBound() > m_upperBound)
//  {
//     scaleDiv.setUpperBound(m_upperBound);
//     scaleDiv.setLowerBound(m_upperBound - scaleDiv.range());

//     if(scaleDiv.lowerBound() < m_lowerBound ||
//             qFuzzyCompare(scaleDiv.range(), m_upperBound - m_lowerBound))
//         scaleDiv.setLowerBound(m_lowerBound);

//     m_chartDisplay->setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);
//  }
}

void wtss::tl::wtss_dialog::load_settings()
{
  try
  {
     j_config =
         wtss::tl::server_manager::getInstance().loadSettings().object();

     for(QJsonObject::iterator it = j_config.begin();
         it != j_config.end(); ++it)
       add_server(it.key());

     QString startDate = wtss::tl::server_manager::getInstance().
             getDateFilter().find("start_date").value().toString();

     QString endDate = wtss::tl::server_manager::getInstance().
             getDateFilter().find("end_date").value().toString();

     m_ui->m_startDateEdit->setDate(QDate::fromString(startDate, "dd/MM/yyyy"));

     m_ui->m_endDateEdit->setDate(QDate::fromString(endDate, "dd/MM/yyyy"));
  }
  catch(...)
  {
    QMessageBox::warning(this, tr("Web Time Series"),
                         tr("An error has occurred, "
                            "when try to load settings."));
  }
}

void wtss::tl::wtss_dialog::add_server(QString server)
{
  QJsonObject j_server = j_config.find(server).value().toObject();
  QTreeWidgetItem *serverItem =
      new QTreeWidgetItem(m_ui->m_serverTreeWidget, tree_item_type::server);

  serverItem->setText(0, server);

  serverItem->setFlags(serverItem->flags() | Qt::ItemIsUserCheckable);
  bool active = j_server.find("active").value().toBool();
  if(active)
    serverItem->setCheckState(0, Qt::Checked);
  else
    serverItem->setCheckState(0, Qt::Unchecked);

  add_coverage(serverItem);
}

void wtss::tl::wtss_dialog::add_coverage(QTreeWidgetItem *server)
{
  QJsonObject j_server = j_config.find(server->text(0)).value().toObject();
  QJsonObject j_coverages = j_server["coverages"].toObject();

  for(QJsonObject::iterator it = j_coverages.begin(); it != j_coverages.end();
      it++)
  {
    bool active = it.value().toObject().find("active").value().toBool();
    QTreeWidgetItem *coverageItem =
        new QTreeWidgetItem(tree_item_type::coverage);

    coverageItem->setText(0, it.key());

    coverageItem->setFlags(coverageItem->flags() | Qt::ItemIsUserCheckable);

    if(active)
      coverageItem->setCheckState(0, Qt::Checked);
    else
      coverageItem->setCheckState(0, Qt::Unchecked);

    server->addChild(coverageItem);

    add_atributes(coverageItem, it.value().toObject());
  }
}

void wtss::tl::wtss_dialog::add_atributes(QTreeWidgetItem *coverageItem,
                                          QJsonObject j_coverage)
{
  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();

  for(QJsonObject::iterator it = j_attributes.begin(); it != j_attributes.end();
      it++)
  {
    QString attribute = it.key();

    bool active = it.value().toObject().find("active").value().toBool();
    QTreeWidgetItem *attributeItem =
        new QTreeWidgetItem(tree_item_type::attribute);

    attributeItem->setText(0, attribute);
    attributeItem->setFlags(attributeItem->flags() | Qt::ItemIsUserCheckable);

    if(active)
      attributeItem->setCheckState(0, Qt::Checked);
    else
      attributeItem->setCheckState(0, Qt::Unchecked);

    coverageItem->addChild(attributeItem);
  }
}

bool wtss::tl::wtss_dialog::validate_query()
{
  if(m_checkServer)
  {
    if(m_checkCoverage)
    {
      if(m_checkAttribute)
        return true;
      else
      {
        QMessageBox::warning(this, tr("Web Time Series"),
                             tr("Please, select an attribute."));
        return false;
      }
    }
    else
    {
      QMessageBox::warning(this, tr("Web Time Series"),
                           tr("Please, select a coverage."));
      return false;
    }
  }
  else
  {
    QMessageBox::warning(this, tr("Web Time Series"), tr("Please, select a server"));
    return false;
  }
}

void wtss::tl::wtss_dialog::do_timeseries_query(
    wtss::cxx::timeseries_query_t query)
{
  clear_canvas();

  m_ui->latLineEdit->setText(QString::number(query.latitude));
  m_ui->longLineEdit->setText(QString::number(query.longitude));

  te::qt::widgets::ScopedCursor c(Qt::WaitCursor);

  QJsonObject j_object =
      wtss::tl::server_manager::getInstance().loadSettings().object();

  if(j_object.isEmpty())
    return;

  QJsonObject j_server;

  m_checkServer = false;
  m_checkCoverage = false;
  m_checkAttribute = false;

  for(QJsonObject::iterator it_server = j_object.begin();
      it_server != j_object.end(); ++it_server)
  {
    j_server = it_server.value().toObject();

    if(j_server.find("active").value().toBool())
    {
      std::string server_uri = it_server.key().toStdString();

      QJsonObject j_coverages = j_server["coverages"].toObject();

      for(QJsonObject::iterator it_coverages = j_coverages.begin();
          it_coverages != j_coverages.end(); it_coverages++)
      {
        if(it_coverages.value().toObject().find("active").value().toBool())
        {
          wtss::cxx::timeseries_query_t new_q;

          new_q.latitude = query.latitude;
          new_q.longitude = query.longitude;
          new_q.coverage_name = it_coverages.key().toUtf8().data();

          QJsonObject j_attributes = it_coverages.value()
                                         .toObject()
                                         .find("attributes")
                                         .value()
                                         .toObject();

          for(QJsonObject::iterator it_attributes = j_attributes.begin();
              it_attributes != j_attributes.end(); it_attributes++)
          {
            QJsonObject j_attribute = it_attributes.value().toObject();

            if(j_attribute["active"].toBool() == true)
            {
              new_q.attributes.push_back(it_attributes.key().toUtf8().data());
              m_checkAttribute = true;
            }
          }

          if(new_q.attributes.size() > 0)
          {
            wtss::cxx::client remote(server_uri);

            try
            {
              m_result = remote.time_series(new_q);
              m_lastQueriedServer = it_server.key().toUtf8().data();
              convert_to_time_series(m_result);
            }
            catch(const std::exception &e)
            {
              QMessageBox::warning(this, tr("Web Time Series"), tr("The coordinates "
                                                                       "informed are invalid."));
              return;
            }
          }
          m_checkCoverage = true;
        }
      }
      m_checkServer = true;
    }
  }

  if(!validate_query())
  {
    m_chartDisplay->detachItems(te::qt::widgets::TIMESERIES_CHART);
    return;
  }

  add_location(query.longitude, query.latitude);

  plot_time_series();
}

void wtss::tl::wtss_dialog::convert_to_time_series(wtss::cxx::timeseries_query_result_t result)
{
  QString cv_name(QString::fromUtf8(result.coverage.name.c_str()));

  std::vector<wtss::cxx::queried_attribute_t> attributes =
      result.coverage.queried_attributes;

  m_timeSeriesVec.clear();

  for(unsigned int i = 0; i < attributes.size(); i++)
  {
    wtss::cxx::queried_attribute_t attribute = attributes[i];

    m_timeSeriesVec.push_back(new te::st::TimeSeries(attribute.name));

    for(unsigned int j = 0; j < attribute.values.size(); ++j)
    {
      QJsonObject j_attribute =
          wtss::tl::server_manager::getInstance().getAttribute(
              QString::fromUtf8(m_lastQueriedServer.c_str()), cv_name, QString::fromUtf8(attribute.name.c_str()));

       wtss::cxx::date d = result.coverage.timeline[j];
       te::dt::DateTime* time =
           new te::dt::Date(boost::gregorian::date(d.year, d.month, d.day));

       double value = attribute.values[j] * j_attribute["scale_factor"].toDouble();

       m_timeSeriesVec.at(i)->add(time,value);
    }
  }
}

std::vector<te::st::TimeSeries *> wtss::tl::wtss_dialog::get_time_series()
{
  return m_timeSeriesVec;
}

void wtss::tl::wtss_dialog::plot_time_series()
{
  m_chartDisplay->detachItems(te::qt::widgets::TIMESERIES_CHART);

  if(get_time_series().empty())
      return;

  for(unsigned int i = 0; i < get_time_series().size(); ++i)
  {
      m_timeSeriesChart = new te::qt::widgets::TimeSeriesChart(
                  get_time_series().at(i));

      m_timeSeriesChart->setPen(QPen(random_color()));
      m_timeSeriesChart->attach(m_chartDisplay);
      m_timeSeriesChart->setTitle(tr(get_time_series().at(i)->getId().c_str()));

      m_chartDisplay->adjustDisplay();
      m_chartDisplay->show();
      m_chartDisplay->replot();
  }
}

void wtss::tl::wtss_dialog::define_display()
{
  m_chartStyle = new te::qt::widgets::ChartStyle();
  m_chartStyle->setTitle(QString::fromUtf8("Web Time Series"));
  m_chartStyle->setAxisX(QString::fromUtf8("Timeline"));
  m_chartStyle->setAxisY(QString::fromUtf8("Value"));

  m_chartDisplay =
          new te::qt::widgets::ChartDisplay(m_ui->m_timeSeriesFrame,
                                            QString::fromUtf8("Web Time Series"),
                                            m_chartStyle);

  QGridLayout* m_layout = new QGridLayout(m_ui->m_timeSeriesFrame);
  m_layout->addWidget(m_chartDisplay, 0, 0);
  m_layout->setContentsMargins(0,0,0,0);
}

void wtss::tl::wtss_dialog::define_marker()
{
  te::se::Stroke* stroke = te::se::CreateStroke("#000000", "1");
  te::se::Fill* fill = te::se::CreateFill("#000000", "1.0");
  m_marker = te::se::CreateMark("cross", stroke, fill);
  m_rgbaMarker = te::map::MarkRendererManager::getInstance().render(m_marker, 12);
}

void wtss::tl::wtss_dialog::point_picked(QPointF &coord)
{
  emit pointPicked(coord);
}

void wtss::tl::wtss_dialog::add_marker(double x, double y)
{
  m_mapDisplay->getDisplayPixmap()->fill(Qt::transparent);

  const te::gm::Envelope& env = m_mapDisplay->getExtent();

  te::qt::widgets::Canvas canvas(m_mapDisplay->getDraftPixmap());

  canvas.setWindow(env.m_llx, env.m_lly, env.m_urx, env.m_ury);
  canvas.setPointPattern(m_rgbaMarker, 12, 12);

  te::gm::Point point;
  point.setX(x);
  point.setY(y);

  canvas.setPointColor(te::color::RGBAColor(0,0,0,0));
  canvas.draw(&point);

  m_mapDisplay->repaint();
}

QColor wtss::tl::wtss_dialog::random_color()
{
  int r = rand() % 255;
  int g = rand() % 255;
  int b = rand() % 255;
  return QColor(r, g, b);
}

void wtss::tl::wtss_dialog::add_location(double x, double y)
{
   QList<QListWidgetItem *> coord =
           m_ui->m_coordSListWidget->findItems(QString::number(x)+","
                                               +QString::number(y),
                                               Qt::MatchExactly);
   if(coord.length() != 0)
      return;

    m_ui->m_coordSListWidget->addItem(QString::number(x)+","+
                                       QString::number(y));
}

void wtss::tl::wtss_dialog::clear_canvas()
{
  te::qt::widgets::Canvas canvas(m_mapDisplay->getDraftPixmap());

  canvas.clear();

  m_mapDisplay->refresh();
}

void wtss::tl::wtss_dialog::closeEvent(QCloseEvent *e)
{
  m_chartDisplay->detachItems(te::qt::widgets::TIMESERIES_CHART);

  m_ui->m_coordSListWidget->clear();

  clear_canvas();

  emit close();
}
