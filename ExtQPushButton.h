/* Copyright (c) 2019, Nikita Grebenyuk (@nikich340). All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#pragma once

#include <QtWidgets>

class ExtQPushButton : public QPushButton {
Q_OBJECT
private:
    QString str;
    QLabel* pLabel = nullptr;
    QPushButton* pButton = nullptr;
    QVBoxLayout* pLayout = nullptr;
public:
    ExtQPushButton(QPushButton *pPB = nullptr);
    ExtQPushButton(QString s, QPushButton *pPB = nullptr);
    void setText(QString s);
    QString text();
};
