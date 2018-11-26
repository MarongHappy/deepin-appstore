/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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

#include "ui/widgets/title_bar_menu.h"

#include <QDebug>

#include "services/settings_manager.h"

namespace dstore {

TitleBarMenu::TitleBarMenu(bool support_sign_in, QWidget* parent)
    : QMenu(parent),
      support_sign_in_(support_sign_in) {

  this->initActions();
}

TitleBarMenu::~TitleBarMenu() {

}

void TitleBarMenu::setUserInfo(const QVariantMap& info) {
  Q_ASSERT(support_sign_in_);
  auto username = info.value("name").toString();
  is_signed_in_ = !username.isEmpty();
  if (support_sign_in_) {
    if (is_signed_in_) {
      sign_in_action_->setText(QObject::tr("Sign Out"));
    } else {
      sign_in_action_->setText(QObject::tr("Sign In"));
    }
  }
}

void TitleBarMenu::setRegion(bool is_china) {
  if (!AllowSwitchRegion()) {
    qWarning() << "Do not allow switching regions";
    return;
  }
  if (is_china) {
    region_china_->setChecked(true);
  } else {
    region_international_->setChecked(true);
  }
}

void TitleBarMenu::setThemeName(QString theme_name) {
  theme_name_ = theme_name;
  SetThemeName(theme_name_);
  if (theme_name_ == "light") {
    switch_theme_action_->setChecked(false);
  } else {
    switch_theme_action_->setChecked(true);
  }
}

void TitleBarMenu::initActions() {
  if (support_sign_in_) {
    sign_in_action_ = this->addAction(QObject::tr("Sign In"));
    connect(sign_in_action_, &QAction::triggered,
            this, &TitleBarMenu::onSignInActionTriggered);

    this->addAction(QObject::tr("Recommend App"),
                    this, &TitleBarMenu::recommendAppRequested);
  }

  QMenu* region_menu = nullptr;
  if (AllowSwitchRegion()) {
    region_menu = this->addMenu(QObject::tr("Select Region"));
  } else {
    region_menu = new QMenu();
    connect(this, &QObject::destroyed,
            region_menu, &QMenu::deleteLater);
  }
  region_china_ = region_menu->addAction(QObject::tr("China"));
  region_china_->setCheckable(true);
  region_international_ = region_menu->addAction(QObject::tr("International"));
  region_international_->setCheckable(true);
  region_group_ = new QActionGroup(this);
  region_group_->setExclusive(true);
  region_group_->addAction(region_china_);
  region_group_->addAction(region_international_);

  const OperationServerRegion curr_region = GetRegion();
  if (curr_region == RegionChina) {
    region_china_->setChecked(true);
  } else {
    region_international_->setChecked(true);
  }

  connect(region_group_, &QActionGroup::triggered,
          this, &TitleBarMenu::onRegionGroupTriggered);

  this->addAction(QObject::tr("Clear Cache"),
                  this, &TitleBarMenu::clearCacheRequested);

  const QString themeName = GetThemeName();
  switch_theme_action_ = this->addAction(QObject::tr("Dark Theme"));
  switch_theme_action_->setCheckable(true);
  connect(switch_theme_action_, &QAction::triggered,
          this, &TitleBarMenu::onThemeActionTriggered);
  this->setThemeName(themeName);

  this->addSeparator();
}

void TitleBarMenu::onSignInActionTriggered() {
  emit this->loginRequested(!is_signed_in_);
}

void TitleBarMenu::onThemeActionTriggered() {
  if(theme_name_ == "light") {
    theme_name_ = "dark";
  } else {
    theme_name_ = "light";
  }
  this->setThemeName(theme_name_);
  emit this->switchThemeRequested(theme_name_);
}

void TitleBarMenu::onRegionGroupTriggered(QAction* action) {
  OperationServerRegion region = RegionInternational;
  if (action == region_china_) {
    region = RegionChina;
  }
  SetRegion(region);

  emit this->regionChanged();
}

}  // namespace dstore
