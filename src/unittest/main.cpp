/*
  Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.

  This file is part of the WTSS.CXX.

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
  \file unittest/main.cpp

  \brief Unit-test for WTSS.TL.

  \author Matheus Cavassan Zaglia
 */

// WTSS.TL
#include <wtss-cxx/wtss.hpp>
// STL
#include <cstdlib>
#include <iostream>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QFile>
#include <QDebug>
int main(int argc, char* argv[])
{


    //"http://www.dpi.inpe.br/mds"
//  QJsonObject j_object;
//  QJsonObject j_coverages;
//  wtss_cxx::wtss remote("http://www.dpi.inpe.br/mds");
//  std::vector<std::string> result = remote.list_coverages();
//  for(std::string cv_name: result)
//  {
//    wtss_cxx::geoarray_t g_array = remote.describe_coverage(cv_name);
//    QJsonObject j_attributes;
//    QJsonObject j_coverage;

//    j_coverage["active"] = QJsonValue(false);
//    for(wtss_cxx::attribute_t attribute: g_array.attributes){
//        j_attributes[QString::fromStdString(attribute.name)] = QJsonValue(false);
//      }
//    j_coverage["attributes"] = j_attributes;
//    j_coverages[QString::fromStdString(cv_name)] = j_coverage;
//  }
//  j_object[QString::fromStdString("http://www.dpi.inpe.br/mds")] = j_coverages;

//  QJsonValue j = j_object.find("http://www.dpi.inpe.br/mds").value();


  QFile f("/home/mzaglia/.local/share/terraview/servers.json");
  f.open(QIODevice::ReadOnly);
  QJsonParseError j_parse_error;
  QString input_data = f.readAll();
  QJsonDocument j_doc = QJsonDocument::fromJson(input_data.toUtf8(), &j_parse_error);
  f.close();

  QJsonObject j_object = j_doc.object();



  if(!j_object.contains("http://www.dpi.inpe.br/mds"))
    throw;
  QJsonObject j_server = j_object.find("b1").value().toObject();

  if(!j_server.contains("MCD43A4"))
    throw;
  QJsonObject j_coverage = j_server.find("MCD43A4").value().toObject();
  QJsonObject j_attributes = j_coverage.find("attributes").value().toObject();


  if(j_attributes["b1"].toBool())
    j_attributes["b1"] = QJsonValue(false);
  else
    j_attributes["b1"] = QJsonValue(true);

  j_coverage["attributes"] = j_attributes;
  j_server["MCD43A4"] = j_coverage;
  j_object["http://www.dpi.inpe.br/mds"] = j_server;

  j_doc.setObject(j_object);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  out << j_doc.toJson();
  f.close();
  return EXIT_SUCCESS;

}
