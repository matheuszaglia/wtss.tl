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
  \file wtss-tl/wtss_dialog.hpp

  \brief Configuration dialog for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

#ifndef __WTSS_TL_WTSS_DIALOG_HPP__
#define __WTSS_TL_WTSS_DIALOG_HPP__

// STL
#include <memory>
#include <string>

// QT
#include <QColor>
#include <QDialog>
#include <QListWidgetItem>
#include <QObject>
#include <QTreeWidget>
#include <QtGui>

// QCustomPlot
#include "qcustomplot.h"

// wtss.cxx
#include <wtss-cxx/data_types.hpp>

// TerraLib
#include <terralib/qt/widgets/canvas/MapDisplay.h>

namespace Ui
{
  class wtss_dialog_form;
}

namespace wtss
{
  namespace tl
  {
    enum tree_item_type
    {
      server,
      coverage,
      attribute
    };
    class wtss_dialog : public QDialog
    {
      Q_OBJECT

     public:
      wtss_dialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

      ~wtss_dialog();

      void set_map_display(te::qt::widgets::MapDisplay* mapDisplay);

      void do_timeseries_query(wtss::cxx::timeseries_query_t query);

     protected slots:

      void onServerAddButtonClicked();

      void onServerRemoveButtonClicked();

      void onServerRefreshButtonClicked();

      void onItemChecked(QTreeWidgetItem* item);

      void onHideButtonClicked();

      void onHelpButtonClicked();

      void onCloseButtonClicked();

      void onExportGraphClicked();

      void onImportGraphClicked();

      void onQueryButtonClicked();

      void onXAxisRangeChanged(QCPRange range);

     private:

      void load_settings();

      void add_server(QString server);

      void add_coverage(QTreeWidgetItem* server);

      void add_atributes(QTreeWidgetItem* coverageItem, QJsonObject j_coverage);

      void add_result_to_plot(QString server_uri,
                              wtss::cxx::timeseries_query_result_t result);

      void plot_result();

      QColor random_color();

     private:
      bool dirty;

      std::auto_ptr<Ui::wtss_dialog_form> m_ui;

      QJsonObject j_config;

      te::qt::widgets::MapDisplay* m_mapDisplay;

      double lowerBound;

      double upperBound;
    };
  }  // end namespace tl
}  // end namespace wtss

#endif  //__WTSS_TL_WTSS_DIALOG_HPP__
