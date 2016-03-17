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
  \file wtss-tl/services_manager.cpp

  \brief Manage the JSON file for services configuration of the Web Time Series Services plugin.

  \author Matheus Cavassan Zaglia
 */

//Terralib

#include <terralib/qt/af/ApplicationController.h>

// WTSS.TL
#include "services_manager.hpp"

// WTSS.CXX
#include <wtss-cxx/wtss.hpp>

//QT
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QFile>
#include <QTextStream>
//STL
#include <vector>
#include <string>


// EXCEPTIONS TO DO

QJsonDocument wtss_tl::services_manager::loadConfig()
{
  QString path = te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir()+"/wtss_tl_settings.json";
  QFile f(path);
  f.open(QIODevice::ReadOnly);
  QJsonParseError j_parse_error;
  QString input_data = f.readAll();
  QJsonDocument j_doc = QJsonDocument::fromJson(input_data.toUtf8(), &j_parse_error);
  f.close();
  return j_doc;
}

void wtss_tl::services_manager::saveConfig(QJsonDocument j_doc)
{
  QString path = te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir()+"/wtss_tl_settings.json";
  QFile f(path);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  out << j_doc.toJson();
  f.close();
}


void wtss_tl::services_manager::addServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadConfig();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    QJsonObject j_coverages;
    QJsonObject j_server;
    wtss_cxx::wtss remote(server_uri.toStdString());
    std::vector<std::string> result = remote.list_coverages();

    for(std::string cv_name: result)
    {
      wtss_cxx::geoarray_t g_array = remote.describe_coverage(cv_name);

      QJsonObject j_attributes;
      QJsonObject j_coverage;

      for(wtss_cxx::attribute_t attribute: g_array.attributes)
        j_attributes[QString::fromStdString(attribute.name)] = QJsonValue(false);

      j_coverage["attributes"] = j_attributes;
      j_coverages[QString::fromStdString(cv_name)] = j_coverage;
    }
    j_server["coverages"] = j_coverages;
    j_server["active"] = QJsonValue(false);
    j_object[server_uri] = j_server;
    j_doc.setObject(j_object);
    saveConfig(j_doc);
  }
}

void wtss_tl::services_manager::removeServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadConfig();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  j_object.remove(server_uri);
  j_doc.setObject(j_object);
  saveConfig(j_doc);
}

bool wtss_tl::services_manager::getStatusCoverage(const QString &server_uri, const QString &cv_name)
{
  QJsonDocument j_doc = loadConfig();

  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(!j_server["coverages"].toObject().contains(cv_name))
    throw;

  QJsonObject j_coverage = j_server.find(cv_name).value().toObject();

  return j_coverage["active"].toBool();
}

bool wtss_tl::services_manager::getStatusAttribute(const QString &server_uri, const QString &cv_name, const QString &attribute)
{
  QJsonDocument j_doc = loadConfig();

  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(!j_server["coverages"].toObject().contains(cv_name))
    throw;

  QJsonObject j_coverage = j_server["coverages"].toObject().find(cv_name).value().toObject();
  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();

  if(!j_attributes.contains(attribute))
    throw;

  return j_attributes[attribute].toBool();
}


void wtss_tl::services_manager::changeStatusService(const QString &server_uri)
{
  QJsonDocument j_doc = loadConfig();

  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(j_server["active"].toBool())
    j_server["active"] = QJsonValue(false);
  else
    j_server["active"] = QJsonValue(true);
}

void wtss_tl::services_manager::changeStatusCoverage(const QString &server_uri, const QString &cv_name)
{
  QJsonDocument j_doc = loadConfig();

  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  QJsonObject j_coverages = j_server["coverages"].toObject();

  if(!j_coverages.contains(cv_name))
    throw;

  QJsonObject j_coverage = j_coverages.find(cv_name).value().toObject();

  if(j_coverage["active"].toBool())
    j_coverage["active"] = QJsonValue(false);
  else
    j_coverage["active"] = QJsonValue(true);



  j_coverages[cv_name] = j_coverage;
  j_server["coverages"] = j_coverages;
  j_object[server_uri] = j_server;
  j_doc.setObject(j_object);
  saveConfig(j_doc);
}

void wtss_tl::services_manager::changeStatusAttribute(const QString &server_uri, const QString &cv_name, const QString &attribute)
{
  QJsonDocument j_doc = loadConfig();

  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
    throw;

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  QJsonObject j_coverages = j_server["coverages"].toObject();

  if(!j_server["coverages"].toObject().contains(cv_name))
    throw;

  QJsonObject j_coverage = j_coverages.find(cv_name).value().toObject();
  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();

  if(!j_attributes.contains(attribute))
    throw;

  if(j_attributes[attribute].toBool())
    j_attributes[attribute] = QJsonValue(false);
  else
    j_attributes[attribute] = QJsonValue(true);


  j_coverage["attributes"] = j_attributes;
  j_coverages[cv_name] = j_coverage;
  j_server["coverages"] = j_coverages;
  j_object[server_uri] = j_server;
  j_doc.setObject(j_object);
  saveConfig(j_doc);
}
