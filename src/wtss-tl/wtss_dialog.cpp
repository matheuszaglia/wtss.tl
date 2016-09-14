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
#include "wtss_tool.hpp"

// QT
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

// Boost
#include <boost/format.hpp>

// TerraLib
#include <terralib/qt/widgets/utils/ScopedCursor.h>

// STL
#include <ctime>
#include <fstream>

// wtss.cxx
#include <wtss-cxx/wtss.hpp>

wtss::tl::wtss_dialog::wtss_dialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), dirty(false), m_ui(new Ui::wtss_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Services"));
  m_ui->btnAddServer->setIcon(QIcon::fromTheme("list-add"));
  m_ui->btnRemoveServer->setIcon(QIcon::fromTheme("list-remove"));
  m_ui->btnRefreshServer->setIcon(QIcon::fromTheme("view-refresh"));
  m_ui->m_hideToolButton->setIcon(QIcon("share/wtss-tl/icons/hide.svg"));

  srand(time(NULL));

  connect(m_ui->btnAddServer, SIGNAL(clicked()), this,
          SLOT(onServerAddButtonClicked()));
  connect(m_ui->btnRemoveServer, SIGNAL(clicked()), this,
          SLOT(onServerRemoveButtonClicked()));
  connect(m_ui->m_serverTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
          this, SLOT(onItemChecked(QTreeWidgetItem *)));
  connect(m_ui->btnHelp, SIGNAL(clicked()), this, SLOT(onHelpButtonClicked()));
  connect(m_ui->btnClose, SIGNAL(clicked()), this,
          SLOT(onCloseButtonClicked()));
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
  connect(m_ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this,
          SLOT(onXAxisRangeChanged(QCPRange)));

  //  j_config =
  //  wtss::tl::server_manager::getInstance().loadSettings().object();
  //  for (QJsonObject::iterator it = j_config.begin(); it != j_config.end();
  //  ++it)
  //    add_server(it.key());

  load_settings();

  dirty = true;
}

wtss::tl::wtss_dialog::~wtss_dialog() {}
void wtss::tl::wtss_dialog::set_map_display(
    te::qt::widgets::MapDisplay *mapDisplay)
{
  m_mapDisplay = mapDisplay;
}

void wtss::tl::wtss_dialog::do_timeseries_query(
    wtss::cxx::timeseries_query_t query)
{
  m_ui->latLineEdit->setText(QString::number(query.latitude));
  m_ui->longLineEdit->setText(QString::number(query.longitude));
  m_ui->customPlot->clearGraphs();
  te::qt::widgets::ScopedCursor c(Qt::WaitCursor);
  QJsonObject j_object =
      wtss::tl::server_manager::getInstance().loadSettings().object();

  QJsonObject j_server;

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
              new_q.attributes.push_back(it_attributes.key().toUtf8().data());
          }

          if(new_q.attributes.size() > 0)
          {
            wtss::cxx::client remote(server_uri);

            try
            {
              wtss::cxx::timeseries_query_result_t result =
                  remote.time_series(new_q);
              add_result_to_plot(it_server.key(), result);
            }
            catch(const std::exception &e)
            {
              boost::format err_msg(
                  "Could not retrieve the time series of the coverage "
                  "%1%.");
              throw exception()
                  << error_description((err_msg % new_q.coverage_name).str());
            }
          }
        }
      }
    }
  }

  plot_result();
}

void wtss::tl::wtss_dialog::onServerAddButtonClicked()
{
  dirty = false;

  QInputDialog *inputDialog = new QInputDialog();
  inputDialog->setOptions(QInputDialog::NoButtons);

  bool ok;

  QString uri = inputDialog->getText(NULL, "Add Server", "Server URI:",
                                     QLineEdit::Normal, "", &ok);
  if(!uri.isEmpty())
  {
    te::qt::widgets::ScopedCursor c(Qt::WaitCursor);

    wtss::tl::server_manager::getInstance().addServer(uri);

    j_config = wtss::tl::server_manager::getInstance().loadSettings().object();

    add_server(uri);
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
      wtss::tl::server_manager::getInstance().removeServer(
          m_ui->m_serverTreeWidget->currentItem()->text(0));

      j_config =
          wtss::tl::server_manager::getInstance().loadSettings().object();

      delete m_ui->m_serverTreeWidget->currentItem();
    }
  }
  dirty = true;
}

void wtss::tl::wtss_dialog::onServerRefreshButtonClicked()
{
  dirty = false;
  if(m_ui->m_serverTreeWidget->currentItem() &&
     !m_ui->m_serverTreeWidget->currentItem()->parent())
  {
    QString uri = m_ui->m_serverTreeWidget->currentItem()->text(0);

    wtss::tl::server_manager::getInstance().refreshServer(uri);

    delete m_ui->m_serverTreeWidget->currentItem();

    add_server(uri);
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

void wtss::tl::wtss_dialog::onHelpButtonClicked() {}

void wtss::tl::wtss_dialog::onCloseButtonClicked() { this->close(); }

void wtss::tl::wtss_dialog::onExportGraphClicked()
{
  QCPDataMap *graph_data = m_ui->customPlot->graph()->data();
  for(int i = 0; i < m_ui->customPlot->graphCount(); ++i)
  {
    QString csvFile = QFileDialog::getSaveFileName(
        this, tr("Save File"),
        QDir::currentPath() + "/" + m_ui->customPlot->graph(i)->name() + ".csv",
        "CSV file (*.csv)");
    if(csvFile.isEmpty()) return;

    QFileInfo info(csvFile);

    if(info.suffix().isEmpty()) csvFile.append(".csv");

    std::ofstream myfile(csvFile.toUtf8().data());

    myfile << "Timeline,Value" << std::endl;

    for(auto it = graph_data->begin(); it != graph_data->end(); ++it)
    {
      QDateTime date;

      auto key = it.key();
      date.setTimeSpec(Qt::UTC);
      date.setTime_t(key);
      QString string_date = date.toString("yyyy/MM/dd");
      auto value = it.value().value;
      myfile << string_date.toUtf8().data() << "," << value << std::endl;
    }
    myfile.close();
  }
}

void wtss::tl::wtss_dialog::onImportGraphClicked()
{
  QString csvFile =
      QFileDialog::getOpenFileName(this, "Select a TimeSeries CSV file",
                                   QDir::currentPath(), "CSV file (*.csv)");

  if(csvFile.isEmpty()) return;

  QFileInfo file(csvFile);
  std::ifstream csv(csvFile.toUtf8().data());

  std::string str;
  std::getline(csv, str);
  m_ui->customPlot->addGraph()->setName(file.baseName());
  int i = 0;
  while(std::getline(csv, str))
  {
    QString qstr = QString::fromUtf8(str.c_str());
    QStringList fields = qstr.split(',');
    QDateTime date = QDateTime::fromString(fields.at(0), "yyyy/MM/dd");
    date.setTimeSpec(Qt::UTC);
    double value = fields.at(1).toDouble();
    m_ui->customPlot->graph(m_ui->customPlot->graphCount() - 1)
        ->addData(date.toTime_t(), value);
    i++;
  }
  m_ui->customPlot->graph(m_ui->customPlot->graphCount() - 1)
      ->setPen(QPen(random_color()));
  plot_result();
}

void wtss::tl::wtss_dialog::onQueryButtonClicked()
{
  wtss::cxx::timeseries_query_t query;

  if(m_ui->latLineEdit->text().isEmpty() &&
     m_ui->longLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"),
                         tr("Input the latitude and longitude coordinate."));
    return;
  }
  else if(m_ui->latLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"),
                         tr("Input the latitude coordinate."));
    return;
  }
  else if(m_ui->longLineEdit->text().isEmpty())
  {
    QMessageBox::warning(this, tr("Warning"),
                         tr("Input the longitude coordinate."));
    return;
  }
  else
  {
    query.latitude = m_ui->latLineEdit->text().toDouble();
    query.longitude = m_ui->longLineEdit->text().toDouble();
    do_timeseries_query(query);
  }
}

void wtss::tl::wtss_dialog::load_settings()
{
  j_config = wtss::tl::server_manager::getInstance().loadSettings().object();
  for(QJsonObject::iterator it = j_config.begin(); it != j_config.end(); ++it)
    add_server(it.key());
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

void wtss::tl::wtss_dialog::add_result_to_plot(
    QString server_uri, wtss::cxx::timeseries_query_result_t result)
{
  QString cv_name(QString::fromUtf8(result.coverage.name.c_str()));

  std::vector<wtss::cxx::queried_attribute_t> attributes =
      result.coverage.queried_attributes;

  QVector<double> timeline(result.coverage.timeline.size(), 0);

  for(int i = 0; i < timeline.size(); ++i)
  {
    wtss::cxx::date d = result.coverage.timeline[i];
    QDateTime start = QDateTime(QDate(d.year, d.month, d.day));
    start.setTimeSpec(Qt::UTC);
    timeline[i] = start.toTime_t();
  }

  for(unsigned int i = 0; i < attributes.size(); i++)
  {
    wtss::cxx::queried_attribute_t attribute = attributes[i];

    m_ui->customPlot->addGraph();
    QString legend =
        cv_name + " - " + QString::fromUtf8(attribute.name.c_str());
    m_ui->customPlot->graph(m_ui->customPlot->graphCount() - 1)
        ->setName(legend);

    for(unsigned int j = 0; j < attribute.values.size(); ++j)
    {
      QJsonObject j_attribute =
          wtss::tl::server_manager::getInstance().getAttribute(
              server_uri, cv_name, QString::fromUtf8(attribute.name.c_str()));

      m_ui->customPlot->graph(i)->addData(
          timeline[j],
          j_attribute["scale_factor"].toDouble() * attribute.values[j]);
    }

    m_ui->customPlot->graph(i)->setPen(QPen(random_color()));
  }
}

void wtss::tl::wtss_dialog::plot_result()
{
  // configuring graph visual
  m_ui->customPlot->xAxis->setLabel("Timeline");
  m_ui->customPlot->yAxis->setLabel("Values");
  // configure bottom axis to show date and time instead of number:
  m_ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  m_ui->customPlot->xAxis->setDateTimeFormat("MMM\nyy");
  // set a more compact font size for bottom and left axis tick labels:
  m_ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
  m_ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
  // set a fixed tick-step to one tick per month:
  m_ui->customPlot->xAxis->setAutoTickStep(false);
  m_ui->customPlot->xAxis->setTickStep(2628000);
  m_ui->customPlot->xAxis->setSubTickCount(3);
  // apply manual tick and tick label for y axis:
  m_ui->customPlot->yAxis->setAutoTickStep(true);
  m_ui->customPlot->yAxis->setAutoTickLabels(true);
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
  // set interactions
  m_ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iSelectPlottables |
                                    QCP::iRangeZoom);
  m_ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal);
  m_ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
  m_ui->customPlot->rescaleAxes();
  lowerBound = m_ui->customPlot->xAxis->range().lower;
  upperBound = m_ui->customPlot->xAxis->range().upper;
  m_ui->customPlot->xAxis->setRange(lowerBound, lowerBound + 2628000 * 13);
  m_ui->customPlot->replot();
}

QColor wtss::tl::wtss_dialog::random_color()
{
  int r = rand() % 255;
  int g = rand() % 255;
  int b = rand() % 255;
  return QColor(r, g, b);
}

void wtss::tl::wtss_dialog::onXAxisRangeChanged(QCPRange range)
{
  QCPRange fixedRange(range);
  if(fixedRange.lower < lowerBound)
  {
    fixedRange.lower = lowerBound;
    fixedRange.upper = lowerBound + range.size();
    if(fixedRange.upper > upperBound ||
       qFuzzyCompare(range.size(), upperBound - lowerBound))
      fixedRange.upper = upperBound;
    m_ui->customPlot->xAxis->setRange(fixedRange);
  }
  else if(fixedRange.upper > upperBound)
  {
    fixedRange.upper = upperBound;
    fixedRange.lower = upperBound - range.size();
    if(fixedRange.lower < lowerBound ||
       qFuzzyCompare(range.size(), upperBound - lowerBound))
      fixedRange.lower = lowerBound;
    m_ui->customPlot->xAxis->setRange(fixedRange);
  }
}
