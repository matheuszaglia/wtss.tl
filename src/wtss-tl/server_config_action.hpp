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


#ifndef __WTSS_TL_SERVER_CONFIG_ACTION_HPP__
#define __WTSS_TL_SERVER_CONFIG_ACTION_HPP__

//wtss.tl
#include "abstract_action.hpp"

namespace wtss_tl
{
  class server_config_action : public wtss_tl::abstract_action
  {
    Q_OBJECT
    public:

    server_config_action(QMenu* menu);
    virtual ~server_config_action();

    protected slots:

    virtual void onActionActivated();
  };
}


#endif //__WTSS_TL_SERVER_CONFIG_ACTION_HPP__
