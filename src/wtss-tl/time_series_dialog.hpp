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


#ifndef __WTSS_TL_TIME_SERIES_DIALOG_HPP__
#define __WTSS_TL_TIME_SERIES_DIALOG_HPP__

// STL
#include <memory>
#include <string>
//QT
#include <QDialog>
#include <QObject>
#include <QtGui>
#include <QListWidgetItem>
//wtss.tl

namespace Ui { class time_series_dialog_form; }

namespace wtss_tl{
  class time_series_dialog : public QDialog
  {
    Q_OBJECT

    public:

    time_series_dialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

    ~time_series_dialog();

    protected slots:


  private:

      std::auto_ptr<Ui::time_series_dialog_form> m_ui;

  };
}

#endif //__WTSS_TL_TIME_SERIES_DIALOG_HPP__
