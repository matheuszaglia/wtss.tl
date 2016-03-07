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


#ifndef __WTSS_TL_TIME_SERIES_ACTION_HPP__
#define __WTSS_TL_TIME_SERIES_ACTION_HPP__

//QT
#include <QAction>
#include <QToolBar>
#include <QObject>

#include <string>

namespace te
{
  namespace qt
  {
    namespace af
    {
      namespace evt
      {
        struct Event;
      }
    }
  }
}


namespace wtss_tl
{
  class time_series_action : public QObject
  {
    Q_OBJECT
    public:

      time_series_action(QToolBar* toolbar);
      virtual ~time_series_action();

    protected:
      void createAction(std::string name, std::string pixmap);

    protected slots:

      virtual void onActionToggled(bool checked);

    Q_SIGNALS:

      void triggered(te::qt::af::evt::Event* e);

    protected:

      QToolBar* m_toolBar;
      QAction* m_action;

  };
}


#endif //__WTSS_TL_TIME_SERIES_ACTION_HPP__
