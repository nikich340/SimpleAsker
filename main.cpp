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

#include "SimpleAsker.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    SimpleAsker asker;

    QBrush brush;
    QPalette pallete = asker.palette();
    QPixmap backgroundPix = QPixmap(":/common/background.jpg");

    brush.setTexture(backgroundPix.scaled(asker.screenW, asker.screenH, Qt::KeepAspectRatioByExpanding));
    pallete.setBrush(QPalette::Background, brush);
    asker.setPalette(pallete);

    asker.showMaximized();
    return app.exec();
}
