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
#include "server_config_dialog.hpp"
#include "ui_server_config_dialog_form.h"
//QT
#include <QInputDialog>
#include <iostream>
wtss_tl::server_config_dialog::server_config_dialog(QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
m_ui(new Ui::server_config_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Servers"));


  connect(m_ui->btnAddServer, SIGNAL(clicked()),this,SLOT(onServerAddButtonClicked()));
  connect(m_ui->listServer, SIGNAL(clicked()),this,SLOT(onServerRemoveButtonClicked()));
}

wtss_tl::server_config_dialog::~server_config_dialog()
{

}

void wtss_tl::server_config_dialog::onServerAddButtonClicked()
{
  server_t server;
  QInputDialog* inputDialog = new QInputDialog();
  inputDialog->setOptions(QInputDialog::NoButtons);
  bool ok;
  server.uri = inputDialog->getText(NULL,"Add Server","Server URI:",QLineEdit::Normal,"", &ok).toStdString();
  if(!server.uri.empty())
  {
    servers.push_back(server);
    QListWidgetItem* server_w = new QListWidgetItem(QString::fromStdString(server.uri),m_ui->listServer);
//    server_w->setFlags(server_w->flags() | Qt::ItemIsUserCheckable);
//    server_w->setCheckState(Qt::Unchecked);
  }
}

void wtss_tl::server_config_dialog::onServerRemoveButtonClicked()
{

}
