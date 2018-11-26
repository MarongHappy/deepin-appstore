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

#include "ui/channel/menu_proxy.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonObject>

namespace dstore
{

MenuProxy::MenuProxy(QObject *parent) : QObject(parent)
{
    this->setObjectName("MenuProxy");
}

MenuProxy::~MenuProxy()
{

}

void MenuProxy::setUserInfo(QVariantMap info)
{
    emit this->userInfoUpdated(info);
}

}  // namespace dstore
