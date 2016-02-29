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
#include <QMessageBox>

#include <iostream>

//wtss.cxx
#include <wtss-cxx/wtss.hpp>

wtss_tl::server_config_dialog::server_config_dialog(QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
m_ui(new Ui::server_config_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Servers"));

  connect(m_ui->btnAddServer, SIGNAL(clicked()),this,SLOT(onServerAddButtonClicked()));
  connect(m_ui->btnRemoveServer, SIGNAL(clicked()),this,SLOT(onServerRemoveButtonClicked()));
  connect(m_ui->listServer,SIGNAL(itemSelectionChanged()),SLOT(onListServerItemSelected()));
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
    std::string s_key = server.uri;
    //wtss_cxx::wtss remote("http://dpi.inpe.br/mds");

    new QListWidgetItem(QString::fromStdString(server.uri),m_ui->listServer);
  }
}

void wtss_tl::server_config_dialog::onServerRemoveButtonClicked()
{
  if(m_ui->listServer->selectedItems().length() > 0)
  {
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Remove server", "Remove '"+
                                    m_ui->listServer->currentItem()->text() +"' server?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {
//        servers.erase(m_ui->listServer->currentItem()->text().toStdString());
        delete m_ui->listServer->currentItem();
      }
  }

}

void wtss_tl::server_config_dialog::onListServerItemSelected()
{
  if(m_ui->listServer->selectedItems().length() > 0)
  {
    std::string server_uri = m_ui->listServer->currentItem()->text().toStdString();

//    QListWidgetItem* item = new QListWidgetItem("item", listWidget);
//    item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
//    item->setCheckState(Qt::Unchecked); // AND initialize check state
  }
}
