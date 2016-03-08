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

/*!
  \file wtss-tl/server_config_dialog.cpp

  \brief Configuration dialog for Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */


//wtss.cxx
#include <wtss-cxx/wtss.hpp>

//wtss.tl
#include "server_config_dialog.hpp"
#include "ui_server_config_dialog_form.h"
#include "services_manager.hpp"

//QT
#include <QInputDialog>
#include <QMessageBox>

//boost
#include <boost/algorithm/string/join.hpp>

wtss_tl::server_config_dialog::server_config_dialog(QWidget *parent, Qt::WindowFlags f):
QDialog(parent, f),
m_ui(new Ui::server_config_dialog_form)
{
  m_ui->setupUi(this);
  this->setWindowTitle(tr("Web Time Series Services - Settings"));

  QJsonDocument j_doc = wtss_tl::services_manager::getInstance().loadConfig();
  j_config = j_doc.object();
  if(j_config.keys().size() > 0)
    m_ui->listServer->addItems(j_config.keys());

  m_ui->btnAddServer->setIcon(QIcon::fromTheme("list-add"));
  m_ui->btnRemoveServer->setIcon(QIcon::fromTheme("list-remove"));

  connect(m_ui->btnAddServer, SIGNAL(clicked()),this,SLOT(onServerAddButtonClicked()));
  connect(m_ui->btnRemoveServer, SIGNAL(clicked()),this,SLOT(onServerRemoveButtonClicked()));
  connect(m_ui->listServer,SIGNAL(itemSelectionChanged()),SLOT(onListServerItemSelected()));
  connect(m_ui->listCoverages,SIGNAL(itemChanged(QListWidgetItem*)),SLOT(onListCoverageChecked(QListWidgetItem*)));
}

wtss_tl::server_config_dialog::~server_config_dialog()
{

}

void wtss_tl::server_config_dialog::onServerAddButtonClicked()
{
  QInputDialog* inputDialog = new QInputDialog();
  inputDialog->setOptions(QInputDialog::NoButtons);
  bool ok;
  QString uri = inputDialog->getText(NULL,"Add Server","Server URI:",QLineEdit::Normal,"", &ok);
  if(!uri.isEmpty())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    wtss_tl::services_manager::getInstance().addServer(uri);
    QApplication::restoreOverrideCursor();
    m_ui->listServer->addItem(uri);
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
        wtss_tl::services_manager::getInstance().removeServer(m_ui->listServer->currentItem()->text());
        delete m_ui->listServer->currentItem();
        m_ui->listCoverages->clear();
        m_ui->listAttributes->clear();
      }
  }
}

//NEEDS IMPROVEMENT
void wtss_tl::server_config_dialog::onListServerItemSelected()
{
  if(m_ui->listServer->selectedItems().length() > 0)
  {
    m_ui->listCoverages->clear();
    m_ui->listAttributes->clear();
    server_uri = m_ui->listServer->currentItem()->text();
    QJsonObject j_coverages = j_config.find(server_uri).value().toObject();
    QJsonObject::iterator it;
    for(it = j_coverages.begin(); it != j_coverages.end(); it++)
    {
      QString coverage = it.key();
      QJsonObject j_coverage = it.value().toObject();
      QListWidgetItem* c = new QListWidgetItem(coverage, m_ui->listCoverages);
      c->setFlags(c->flags() | Qt::ItemIsUserCheckable); // set checkable flag
      if(j_coverage["active"].toBool())
      {
        c->setCheckState(Qt::Checked);
        QJsonObject::iterator it_at;
        QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();
        for(it_at = j_attributes.begin(); it_at != j_attributes.end(); it_at++)
        {
          QString attribute = it_at.key();
          bool active = it_at.value().toBool();
          QListWidgetItem* a = new QListWidgetItem(attribute, m_ui->listAttributes);
          a->setFlags(a->flags() | Qt::ItemIsUserCheckable);
          if(active)
            a->setCheckState(Qt::Checked);
          else
            a->setCheckState(Qt::Unchecked);
        }
      }
      else
        c->setCheckState(Qt::Unchecked);
    }
  }
}

void wtss_tl::server_config_dialog::onListCoverageChecked(QListWidgetItem* item)
{
  wtss_tl::services_manager::getInstance().changeStatusCoverage(server_uri, item->text());
}


