/*
 * Copyright (C) 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "services/search_manager.h"

#include <QDebug>

#include "services/backend/chinese2pinyin.h"

namespace dstore {

namespace {

const int kMaxSearchResult = 10;

SearchMetaList SearchApp(const QString& keyword,
                              const SearchMetaList& apps,
                              const QStringList& app_names_pinyin) {
  SearchMetaList result;
  QSet<QString> app_names;
  const QString keyword_pinyin = Chinese2PinyinNoSyl(keyword);

  for (int i = 0; i < app_names_pinyin.length(); i++) {
    if (app_names_pinyin.at(i).contains(keyword_pinyin, Qt::CaseInsensitive)) {
      result.append(apps.at(i));
      app_names.insert(apps.at(i).name);
    }
  }

  for (const SearchMeta& app : apps) {
    if (app.name.contains(keyword_pinyin, Qt::CaseInsensitive) ||
        app.local_name.contains(keyword_pinyin, Qt::CaseInsensitive)) {
      if (!app_names.contains(app.name)) {
        result.append(app);
        app_names.insert(app.name);
      }
    }
  }

  for (const SearchMeta& app : apps) {
    if (app.description.contains(keyword, Qt::CaseInsensitive) ||
        app.slogan.contains(keyword, Qt::CaseInsensitive)) {
      if (!app_names.contains(app.name)) {
        result.append(app);
        app_names.insert(app.name);
      }
    }
  }
  return result;
}

}  // namespace

SearchManager::SearchManager(QObject* parent)
    : QObject(parent),
      app_list_(),
      app_names_pinyin_() {
  this->setObjectName("SearchManager");
}

SearchManager::~SearchManager() {

}

void SearchManager::searchApp(const QString& keyword) {
  SearchMetaList result = SearchApp(keyword, app_list_, app_names_pinyin_);
  result = result.mid(0, kMaxSearchResult);
  emit this->searchAppResult(keyword, result);
}

void SearchManager::completeSearchApp(const QString& keyword) {
  SearchMetaList result = SearchApp(keyword, app_list_, app_names_pinyin_);
  emit this->completeSearchAppResult(keyword, result);
}

void SearchManager::updateAppList(const SearchMetaList& app_list) {
  app_list_ = app_list;
  app_names_pinyin_.clear();

  // Sort application list by appName.
  std::sort(app_list_.begin(), app_list_.end());

  // Save app name pinyin.
  for (const SearchMeta& app : app_list_) {
    app_names_pinyin_.append(Chinese2PinyinNoSyl(app.local_name));
  }
}

}  // namespace dstore