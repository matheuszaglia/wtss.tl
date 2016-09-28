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
  with WTSS.TL. See COPYING. If not, see
  <http://www.gnu.org/licenses/lgpl-3.0.html>.
 */

/*!
  \file wtss-tl/server_manager.cpp

  \brief Manage the JSON file for services configuration of the Web Time Series
  Services plugin.

  \author Matheus Cavassan Zaglia
 */

// WTSS.TL
#include "server_manager.hpp"
#include "exception.hpp"

// Boost
#include <boost/format.hpp>

// QT
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTextStream>

// STL
#include <string>
#include <vector>

// Terralib
#include <terralib/qt/af/ApplicationController.h>

// WTSS.CXX
#include <wtss-cxx/wtss.hpp>

QJsonDocument wtss::tl::server_manager::loadSettings()
{
  QString path = te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir() +
                 "/wtss_settings.json";
  QJsonDocument j_doc;
  QFile f(path);
  if(f.exists())
  {
    f.open(QIODevice::ReadOnly);
    QJsonParseError j_parse_error;
    QString input_data = f.readAll();
    j_doc = QJsonDocument::fromJson(input_data.toUtf8(), &j_parse_error);
    f.close();
  }
  return j_doc;
}

void wtss::tl::server_manager::saveSettings(QJsonDocument j_doc)
{
  QString path = te::qt::af::AppCtrlSingleton::getInstance().getUserDataDir() +
                 "/wtss_settings.json";
  QFile f(path);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  out << j_doc.toJson();
  f.close();
}

void wtss::tl::server_manager::addServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    QJsonObject j_coverages;
    QJsonObject j_server;

    wtss::cxx::client remote(server_uri.toStdString());
    try
    {
      std::vector<std::string> result = remote.list_coverages();

      for(std::string cv_name : result)
      {
        wtss::cxx::geoarray_t g_array = remote.describe_coverage(cv_name);

        QJsonObject j_attributes;
        QJsonObject j_coverage;

        for(wtss::cxx::attribute_t attribute : g_array.attributes)
        {
          QJsonObject j_attribute;
          j_attribute["active"] = QJsonValue(false);
          j_attribute["scale_factor"] = attribute.scale_factor;
          j_attribute["missing_value"] = attribute.missing_value;
          j_attributes[QString::fromStdString(attribute.name)] = j_attribute;
        }
        j_coverage["active"] = QJsonValue(false);
        j_coverage["attributes"] = j_attributes;
        j_coverages[QString::fromStdString(cv_name)] = j_coverage;
      }

      j_server["active"] = QJsonValue(false);
      j_server["coverages"] = j_coverages;
      j_object[server_uri] = j_server;
      j_doc.setObject(j_object);
      saveSettings(j_doc);
    }
    catch(const std::exception &e)
    {
      boost::format err_msg(
          "The server %1% could not be added.\n"
          "Due to the following error: %2%");

      throw exception() << error_description(
          (err_msg % server_uri.toUtf8().data() % e.what()).str());
    }
  }
}

void wtss::tl::server_manager::refreshServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(j_object.contains(server_uri))
  {
    QJsonObject j_coverages;
    QJsonObject j_server;

    wtss::cxx::client remote(server_uri.toStdString());
    try
    {
      std::vector<std::string> result = remote.list_coverages();

      for(std::string cv_name : result)
      {
        wtss::cxx::geoarray_t g_array = remote.describe_coverage(cv_name);

        QJsonObject j_attributes;
        QJsonObject j_coverage;

        for(wtss::cxx::attribute_t attribute : g_array.attributes)
        {
          QJsonObject j_attribute;
          j_attribute["active"] = QJsonValue(false);
          j_attribute["scale_factor"] = attribute.scale_factor;
          j_attribute["missing_value"] = attribute.missing_value;
          j_attributes[QString::fromStdString(attribute.name)] = j_attribute;
        }
        j_coverage["active"] = QJsonValue(false);
        j_coverage["attributes"] = j_attributes;
        j_coverages[QString::fromStdString(cv_name)] = j_coverage;
      }

      j_server["active"] = QJsonValue(false);
      j_server["coverages"] = j_coverages;
      j_object[server_uri] = j_server;
      j_doc.setObject(j_object);
      saveSettings(j_doc);
    }
    catch(const std::exception &e)
    {
      boost::format err_msg(
          "The server %1% could not reloaded..\n"
          "Due to the following error: %2%");

      throw exception() << error_description(
          (err_msg % server_uri.toUtf8().data() % e.what()).str());
    }
  }
}

void wtss::tl::server_manager::removeServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(j_object.contains(server_uri))
  {
    j_object.remove(server_uri);
    j_doc.setObject(j_object);
    saveSettings(j_doc);
  }
}

QJsonObject wtss::tl::server_manager::getAttribute(const QString &server_uri,
                                                   const QString &cv_name,
                                                   const QString &attribute)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    boost::format err_msg("Could not find the server: %1%");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(!j_server.contains("coverages"))
  {
    boost::format err_msg("The server %1% has no coverages");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  if(!j_server["coverages"].toObject().contains(cv_name))
  {
    boost::format err_msg("The server %1% has no coverage named: %2%");
    throw out_of_range_exception() << error_description(
        (err_msg % server_uri.toUtf8().data() % cv_name.toUtf8().data()).str());
  }

  QJsonObject j_coverage =
      j_server["coverages"].toObject().find(cv_name).value().toObject();

  if(!j_coverage.contains("attributes"))
  {
    boost::format err_msg("The coverage %1% has no attributes.");
    throw out_of_range_exception()
        << error_description((err_msg % cv_name.toUtf8().data()).str());
  }

  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();

  if(!j_attributes.contains(attribute))
  {
    boost::format err_msg("The coverage %1% has no attribute named: %2%.");
    throw out_of_range_exception() << error_description(
        (err_msg % cv_name.toUtf8().data() % attribute.toUtf8().data()).str());
  }

  return j_attributes.find(attribute).value().toObject();
}

void wtss::tl::server_manager::changeStatusServer(const QString &server_uri)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    boost::format err_msg("Could not find the server: %1%");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  bool active = j_server.find("active").value().toBool();
  j_server["active"] = !active;

  if(!active)
  {
    for(QJsonObject::iterator it = j_object.begin(); it != j_object.end(); ++it)
    {
      if(it.key() != server_uri)
      {
        QJsonObject j_sv = it.value().toObject();
        j_sv["active"] = QJsonValue(false);
        j_object[it.key()] = j_sv;
      }
    }
  }
  j_object[server_uri] = j_server;
  j_doc.setObject(j_object);
  saveSettings(j_doc);
}

void wtss::tl::server_manager::changeStatusCoverage(const QString &server_uri,
                                                    const QString &cv_name)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    boost::format err_msg("Could not find the server: %1%");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(!j_server.contains("coverages"))
  {
    boost::format err_msg("The server %1% has no coverages");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_coverages = j_server.find("coverages").value().toObject();
  QJsonObject j_coverage = j_coverages.find(cv_name).value().toObject();
  bool active = j_coverage.find("active").value().toBool();
  j_coverage["active"] = !active;
  if(!active)
  {
    for(QJsonObject::iterator it = j_coverages.begin(); it != j_coverages.end();
        ++it)
    {
      if(it.key() != cv_name)
      {
        QJsonObject j_cv = it.value().toObject();
        j_cv["active"] = QJsonValue(false);
        j_coverages[it.key()] = j_cv;
      }
    }
  }
  if(!j_coverages.contains(cv_name))
  {
    boost::format err_msg("The server %1% has no coverage named: %2%");
    throw out_of_range_exception() << error_description(
        (err_msg % server_uri.toUtf8().data() % cv_name.toUtf8().data()).str());
  }

  j_coverages[cv_name] = j_coverage;
  j_server["coverages"] = j_coverages;
  j_object[server_uri] = j_server;
  j_doc.setObject(j_object);
  saveSettings(j_doc);
}

void wtss::tl::server_manager::changeStatusAttribute(const QString &server_uri,
                                                     const QString &cv_name,
                                                     const QString &attribute)
{
  QJsonDocument j_doc = loadSettings();
  QJsonObject j_object = j_doc.object();

  if(!j_object.contains(server_uri))
  {
    boost::format err_msg("Could not find the server: %1%");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_server = j_object.find(server_uri).value().toObject();

  if(!j_server.contains("coverages"))
  {
    boost::format err_msg("The server %1% has no coverages");
    throw out_of_range_exception()
        << error_description((err_msg % server_uri.toUtf8().data()).str());
  }

  QJsonObject j_coverages = j_server.find("coverages").value().toObject();

  if(!j_coverages.contains(cv_name))
  {
    boost::format err_msg("The server %1% has no coverage named: %2%");
    throw out_of_range_exception() << error_description(
        (err_msg % server_uri.toUtf8().data() % cv_name.toUtf8().data()).str());
  }

  QJsonObject j_coverage = j_coverages.find(cv_name).value().toObject();

  if(!j_coverage.contains("attributes"))
  {
    boost::format err_msg("The coverage %1% has no attributes.");
    throw out_of_range_exception()
        << error_description((err_msg % cv_name.toUtf8().data()).str());
  }

  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();

  if(!j_attributes.contains(attribute))
  {
    boost::format err_msg("The coverage %1% has no attribute named: %2%.");
    throw out_of_range_exception() << error_description(
        (err_msg % cv_name.toUtf8().data() % attribute.toUtf8().data()).str());
  }

  QJsonObject j_attribute = j_attributes.find(attribute).value().toObject();

  if(!j_attribute.contains("active"))
  {
    boost::format err_msg("The coverage %1% has no attribute named: %2%.");
    throw out_of_range_exception() << error_description(
        (err_msg % cv_name.toUtf8().data() % attribute.toUtf8().data()).str());
  }

  j_attribute["active"] = !j_attribute.find("active").value().toBool();

  j_attributes[attribute] = j_attribute;
  j_coverage["attributes"] = j_attributes;
  j_coverages[cv_name] = j_coverage;
  j_server["coverages"] = j_coverages;
  j_object[server_uri] = j_server;
  j_doc.setObject(j_object);
  saveSettings(j_doc);
}
