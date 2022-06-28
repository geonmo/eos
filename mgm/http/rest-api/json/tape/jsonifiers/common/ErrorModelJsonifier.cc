// ----------------------------------------------------------------------
// File: ErrorModelJsonifier.cc
// Author: Cedric Caffy - CERN
// ----------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2013 CERN/Switzerland                                  *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 ************************************************************************/

#include "ErrorModelJsonifier.hh"

EOSMGMRESTNAMESPACE_BEGIN

void ErrorModelJsonifier::jsonify(const ErrorModel* model,
                                  std::stringstream& ss)
{
  Json::Value root;
  jsonify(model, root);
  ss << root;
}

void ErrorModelJsonifier::jsonify(const ErrorModel* model, Json::Value& root)
{
  if (model->getType()) {
    root["type"] = model->getType().value();
  }

  root["title"] = model->getTitle();
  root["status"] = model->getStatus();

  if (model->getDetail()) {
    root["detail"] = model->getDetail().value();
  }
}

EOSMGMRESTNAMESPACE_END
