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

#include "ExtQPushButton.h"

/* PRIVATE FUNCTIONS */

ExtQPushButton::ExtQPushButton(QString s, QPushButton *pPB) : QPushButton(pPB) {
    str = s;

    pLabel = new QLabel;
    pLabel->setWordWrap(true);
    pLabel->setAlignment(Qt::AlignCenter);
    pLabel->setText(str);

    pLayout = new QVBoxLayout;
    pLayout->addWidget(pLabel);
    pLayout->setAlignment(pLabel, Qt::AlignCenter);
    this->setLayout(pLayout);
}
ExtQPushButton::ExtQPushButton(QPushButton *pPB) {
    ExtQPushButton("", pPB);
}
void ExtQPushButton::setText(QString s) {
    str = s;
    // dirty crutch!
    if (pLabel == nullptr) {
        pLabel = new QLabel;
        pLabel->setWordWrap(true);
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setText(str);
    }
    // dirty crutch!
    if (pLayout == nullptr) {
        pLayout = new QVBoxLayout;
        pLayout->addWidget(pLabel);
        this->setLayout(pLayout);
    }
    pLabel->setText(str);
}
QString ExtQPushButton::text() {
    return str;
}
