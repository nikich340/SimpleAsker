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

/* PRIVATE FUNCTIONS */
QDialog* SimpleAsker::createDialog(QString info, QString pix, QString accept, QString reject, bool mod) {
    QDialog* pdlg = new QDialog(this);
    QHBoxLayout* phbox = new QHBoxLayout;
    QVBoxLayout* pvbox = new QVBoxLayout;
    QLabel* plbl = new QLabel("<h3>" + info + "</h3>");
    plbl->setAlignment(Qt::AlignCenter);
    plbl->setWordWrap(true);

    if (accept != "-") {
        ExtQPushButton* pAcc = new ExtQPushButton(accept);
        connect(pAcc, SIGNAL(clicked(bool)), pdlg, SLOT(accept()));
        phbox->addWidget(pAcc);
    }

    if (reject != "-") {
        ExtQPushButton* pRej = new ExtQPushButton(reject);
        connect(pRej, SIGNAL(clicked(bool)), pdlg, SLOT(reject()));
        phbox->addWidget(pRej);
    }

    pvbox->addWidget(plbl);
    if (pix != "-") {
        QLabel* plblPix = new QLabel;
        plblPix->setScaledContents(true);
        plblPix->adjustSize();
        plblPix->setPixmap(QPixmap(pix).scaled(QSize(screenW * 0.7, screenH * 0.7), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        pvbox->addWidget(plblPix);
    }
    pvbox->addLayout(phbox);
    pdlg->setModal(mod);
    pdlg->setLayout(pvbox);

    pdlg->adjustSize();
    pdlg->setMaximumSize(QSize(screenW*0.8, screenH*0.8));
    pdlg->show();
    return pdlg;
}
bool SimpleAsker::isDigit(QChar c) {
    return (c >= '0' && c <= '9');
}
bool SimpleAsker::isSymbol(QChar c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= QChar(L'а') && c <= QChar(L'я')) || (c >= QChar(L'А') && c <= QChar(L'Я'));
}
bool SimpleAsker::isUpper(QChar c) {
    return (c >= 'A' && c <= 'Z');
}
int SimpleAsker::rand(int L, int R) {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    if (R < L)
        return -1;
    return L + qrand() % (R - L + 1);
}
void SimpleAsker::_dbg_start(QString func) {
    QString out = "";
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    dbg_spacing += 3;
    qDebug() << out << "{";
}
void SimpleAsker::_dbg_end(QString func) {
    QString out = "";
    dbg_spacing -= 3;
    upn(i, 1, dbg_spacing) {
        out.push_back(' ');
    }
    out += func;
    qDebug() << "}" << out;
}
void SimpleAsker::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout);
            childLayout->deleteLater();
        }
        if (item != nullptr) {
            delete item;
        }
    }
}
void SimpleAsker::crash(QString reason) {
    qDebug() << "Crash! Reason: " << reason << "\n";
    QDialog* pdlg = createDialog("Crash! Reason: " + reason + "\n", "-", "Quit", "Try ignore", true);
    connect(pdlg, SIGNAL(accepted()), qApp, SLOT(quit()));
    pdlg->exec();
    pdlg->deleteLater();
}
void SimpleAsker::genQuest() {
    _dbg_start(__func__);
    std::default_random_engine dre(QDateTime::currentMSecsSinceEpoch());
    if (q_unusedAsks.empty()) {
        qDebug() << "Empty quest array";
        onFinishOsteoAsk();
        dbg_spacing -= 3;
        return;
    }

    int idx = 0;
    if (m_pCheckRandomize->checkState() == Qt::Checked) {
        idx = rand(0, q_unusedAsks.size() - 1);
    }
    currentAsk = q_unusedAsks[idx];
    //qDebug() << "idx " << idx;
    //qDebug() << "size " << q_unusedAsks[idx].answers.size();
    /*for (int i = 0; i < q_unusedAsks[idx].answers.size(); ++i) {
        qDebug() << q_unusedAsks[idx].answers[i];
    }*/
    //qDebug() << "right " << q_unusedAsks[idx].rightAns;
    std::shuffle(q_unusedAsks[idx].answers.begin(), q_unusedAsks[idx].answers.end(), dre);

    /* set question label */
    m_pLblQuestion->setText(q_unusedAsks[idx].question);

    /* set answer buttons */
    rightAnswers.clear();
    upn(i, 0, q_unusedAsks[idx].answers.size() - 1) {
        m_pBtnAns[i]->setText(q_unusedAsks[idx].answers[i]);
        m_pBtnAns[i]->show();
    }
    for (auto v : q_unusedAsks[idx].rightAns) {
        rightAnswers.push_back(v);
    }
    upn(j, q_unusedAsks[idx].answers.size(), maxAns - 1) {
        m_pBtnAns[j]->hide();
    }
    q_unusedAsks.erase(q_unusedAsks.begin() + idx);
    _dbg_end(__func__);
}
void SimpleAsker::joinAll() {
    QVector<ask> join;
    for (auto v: QSTs) {
        for (auto i : v) {
            bool foundEqual = false;
            for (auto j : join) {
                if (i == j) {
                    foundEqual = true;
                    break;
                }
            }
            if (!foundEqual)
                join.push_back(i);
        }
    }
    QFile out("join.qst");
    out.open(QFile::WriteOnly | QFile::Text);
    QTextStream outts(&out);
    QString outs = "";
    int cnt = 0;
    for (auto i : join) {
        outs += to_str(++cnt) + ". " + i.question;
        for (auto a : i.answers) {
            bool ok = true;
            for (auto ra : i.rightAns) {
                if (a == ra) {
                    ok = false;
                    break;
                }
            }
            if (ok)
                outs += "# " + a;
        }
        for (auto ra : i.rightAns) {
            outs += "@ " + ra;
        }
        outs += "\n";
    }
    outts << outs;
    out.close();
}
void SimpleAsker::setUpObjects() {
    _dbg_start(__func__);
    /* GENERAL */
    m_pLayoutMain = new QVBoxLayout;
    m_pLayoutMenu = new QVBoxLayout;
    m_pLayoutPreAsk = new QVBoxLayout;
    m_pLayoutAsk = new QVBoxLayout;
    m_pLayoutWrong = new QVBoxLayout;
    m_pWidgetMenu = new QWidget;
    m_pWidgetPreAsk = new QWidget;
    m_pWidgetAsk = new QWidget;
    m_pWidgetWrong = new QWidget;
    m_pPlayer = new QMediaPlayer;

    /* WRONG */
    m_pExit = new ExtQPushButton;
    m_pToMenu = new ExtQPushButton;
    m_pResolve = new ExtQPushButton;
    m_pLblPix = new QLabel;
    m_plblInfo = new QLabel;

    connect(m_pExit, SIGNAL(clicked(bool)), qApp, SLOT(quit()));
    connect(m_pToMenu, SIGNAL(clicked(bool)), this, SLOT(onMenu()));
    connect(m_pResolve, SIGNAL(clicked(bool)), this, SLOT(onResolve()));
    m_pLblPix->setAlignment(Qt::AlignCenter);
    m_pLblPix->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    m_plblInfo->setAlignment(Qt::AlignCenter);
    m_plblInfo->setWordWrap(true);
    m_plblInfo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QHBoxLayout* phbox0 = new QHBoxLayout;
    phbox0->addWidget(m_pToMenu);
    phbox0->addWidget(m_pResolve);
    phbox0->addWidget(m_pExit);

    m_pLayoutWrong->addWidget(m_plblInfo);
    m_pLayoutWrong->addWidget(m_pLblPix);
    m_pLayoutWrong->addLayout(phbox0);

    /* MENU */
    upn(i, 0, 3) {
        m_pBtnMenu[i] = new ExtQPushButton;
        m_pBtnMenu[i]->setParent(this);
        m_pLayoutMenu->addWidget(m_pBtnMenu[i]);
    }
    connect(m_pBtnMenu[0], SIGNAL(clicked(bool)), this, SLOT(onPreStartOsteoAsk()));
    connect(m_pBtnMenu[1], SIGNAL(clicked(bool)), this, SLOT(onSettings()));
    connect(m_pBtnMenu[2], SIGNAL(clicked(bool)), this, SLOT(onAboutProgram()));
    connect(m_pBtnMenu[3], SIGNAL(clicked(bool)), qApp, SLOT(quit()));

    /* SETTINGS */
    m_pCheckRus = new QCheckBox;
    m_pDensitySlider = new QSlider(Qt::Horizontal);
    m_pDialogSettings = new QDialog;
    QVBoxLayout* pvbox = new QVBoxLayout;
    ExtQPushButton* pBtnOk = new ExtQPushButton("OK");

    m_pDensitySlider->setRange(screenH / 2, screenH * 2);
    m_pDensitySlider->setSingleStep(1);

    m_pDialogSettings->setWindowOpacity(0.9);
    m_pDialogSettings->setMinimumWidth(screenW * 0.75);
    m_pCheckRus->setTristate(false);
    connect(m_pDensitySlider, SIGNAL(valueChanged(int)), this, SLOT(onSetStyleSheets(int)));
    connect(pBtnOk, SIGNAL(clicked(bool)), m_pDialogSettings, SLOT(accept()));
    pvbox->addWidget(m_pCheckRus);
    pvbox->addWidget(new QLabel(m_bLangRu ? "Масштабирование интерфейса\n(двигайте ползунок)" : "Interface scaling\n(move slider)"));
    pvbox->addWidget(m_pDensitySlider);
    pvbox->addWidget(pBtnOk);
    m_pDialogSettings->setModal(true);
    m_pDialogSettings->setLayout(pvbox);

    /* PREASK */
    m_pLblStartFrom = new QLabel;
    m_pLineStartFrom = new QLineEdit("1");
    QHBoxLayout *pHboxx = new QHBoxLayout;
    pHboxx->addWidget(m_pLblStartFrom);
    pHboxx->addWidget(m_pLineStartFrom);
    m_pBtnStart = new ExtQPushButton;
    m_pCheckRandomize = new QCheckBox;
    m_pCheckRandomize->setStyleSheet("background-color: rgba(255,255,255,120)");
    m_pLayoutPreAsk->addLayout(pHboxx);
    m_pLayoutPreAsk->addWidget(m_pCheckRandomize);
    m_pLayoutPreAsk->addWidget(m_pBtnStart);

    /* ASK */
    m_pLblQuestion = new QLabel;
    m_pLblInfo = new QLabel;
    m_pBtnNext = new ExtQPushButton;
    m_pBtnFinish = new ExtQPushButton;
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i] = new ExtQPushButton;
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,120)");
        m_pBtnAns[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        connect(m_pBtnAns[i], SIGNAL(clicked(bool)), this, SLOT(onAns()));
    }
    m_pLblQuestion->setWordWrap(true);
    //m_pLblQuestion->setMinimumWidth(screenW * 0.7);
    m_pLblQuestion->setAlignment(Qt::AlignCenter);
    m_pLblInfo->setWordWrap(true);
    m_pLblInfo->setAlignment(Qt::AlignCenter);

    m_pLblQuestion->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_pLblInfo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    //m_pLblInfo->setMinimumWidth(screenW * 0.25);

    QGridLayout* pGridLayout = new QGridLayout;
    QVBoxLayout* pVLayout = new QVBoxLayout;
    upn(i, 0, maxAns - 1) {
        pGridLayout->addWidget(m_pBtnAns[i], i / 2, i % 2);
    }
    //updateInfoLabel();

    pVLayout->addWidget(m_pLblInfo);
    pVLayout->addWidget(m_pLblQuestion);
    pGridLayout->addWidget(m_pBtnFinish, maxAns / 2, 0);
    pGridLayout->addWidget(m_pBtnNext, maxAns / 2, 1);

    m_pLayoutAsk->addLayout(pVLayout);
    m_pLayoutAsk->addLayout(pGridLayout);
    _dbg_end(__func__);
}
void SimpleAsker::readQst(QString path, char type) {
    _dbg_start(__func__);
    QFile in(path);
    if (!in.open(QFile::ReadOnly | QFile::Text)) {
        crash("File " + in.fileName() + " could not be opened in read-only mode");
    }

    QString name = in.readLine();
    QString cur;
    ask tmpAsk;
    QVector<ask> curQst;
    QChar symRight = '@', symWrong = '#';
    if (type == '+') {
        symRight = '+';
        symWrong = '-';
    }
    while (!in.atEnd()) {
        cur = in.readLine();
        int idx = 0, idx2 = 0;
        //qDebug() << "read: " << cur;
        while (cur[idx2] == ' ')
            ++idx2;

        if (cur[idx2] == symRight || cur[idx2] == symWrong) {
            while (cur[idx] != symRight && cur[idx] != symWrong && idx < cur.length())
                ++idx;

            bool isRight = (cur[idx] == symRight);
            idx += 1;

            if (idx >= cur.length())
                continue;

            tmpAsk.answers.push_back(cur.mid(idx));
            if (isRight) {
                tmpAsk.rightAns.push_back(cur.mid(idx));
            }
        } else {
            if (!tmpAsk.question.isEmpty()) {
                if (!tmpAsk.answers.empty()) {
                    if (tmpAsk.rightAns.empty())
                        tmpAsk.question += " (правильный ответ не задан, ищите сами)";
                    curQst.push_back(tmpAsk);
                } else {
                    tmpAsk.question.chop(1);
                    tmpAsk.question += cur;
                    continue;
                    //qDebug() << "testname: " << name << "!!!dropping invalid question: " << tmpAsk.question;
                }
            }
            tmpAsk.clear();
            while (!isSymbol(cur[idx]) && idx < cur.length())
                ++idx;
            if (idx == cur.length()) {
                continue;
            }
            if (tmpAsk.question.isEmpty())
                tmpAsk.question = cur.mid(idx);
            else
                tmpAsk.question += cur.mid(idx);
        }
    }
    if (!tmpAsk.question.isEmpty() && !tmpAsk.answers.empty() && !tmpAsk.rightAns.empty()) {
        curQst.push_back(tmpAsk);
        tmpAsk.clear();
    }

    QSTs.push_back(curQst);

    name.chop(1);
    QSTnames.push_back(name);
    in.close();
    _dbg_end(__func__);
}
void SimpleAsker::updateInfoLabel() {
    m_pLblInfo->setText("<h2>" + QSTnames[choosedQst] + "</h2>" + "\n" +
                        (m_bLangRu ? "Всего: " : "Summary: ") + to_str(q_cnt) + "/" +
                        to_str(q_sum) + (m_bLangRu ? "\nВерно: " : "\nCorrect: ") +
                        to_str(q_rightAnsCnt) + "/" + to_str(q_cnt) + " = " +
                        to_str((qreal)q_rightAnsCnt / (qreal)q_cnt * 100.0) + "%");
}

/* PUBLIC FUNCTIONS */
SimpleAsker::SimpleAsker(QStackedWidget *pswgt) : QStackedWidget(pswgt), m_settings("nikich340", "SimpleAsker")
{
    _dbg_start(__func__);

    m_bLangRu = m_settings.value("/settings/m_bLangRu", true).toBool();
    readQst(":/2_anatomiaALL.qst", '@');

    //joinAll();

    screenH = QGuiApplication::primaryScreen()->geometry().height();
    screenW = QGuiApplication::primaryScreen()->geometry().width();
    setUpObjects();
    onSetStyleSheets(m_settings.value("/settings/customScreenH", screenH).toInt());
    m_pDensitySlider->setValue(customScreenH);

    connect(m_pCheckRus, SIGNAL(stateChanged(int)), this, SLOT(onUpdateLanguage(int)));

    m_pWidgetMenu->setLayout(m_pLayoutMenu);
    m_pWidgetPreAsk->setLayout(m_pLayoutPreAsk);
    m_pWidgetAsk->setLayout(m_pLayoutAsk);
    m_pWidgetWrong->setLayout(m_pLayoutWrong);

    this->addWidget(m_pWidgetMenu);
    this->addWidget(m_pWidgetPreAsk);
    this->addWidget(m_pWidgetAsk);
    this->addWidget(m_pWidgetWrong);
    this->layout()->setSizeConstraint(QLayout::SetMaximumSize);

    onUpdateLanguage(m_bLangRu ? Qt::Checked : Qt::Unchecked);
    updateInfoLabel();
    onMenu();
    _dbg_end(__func__);
}
SimpleAsker::~SimpleAsker() {
    _dbg_start(__func__);
    m_settings.setValue("/settings/m_bLangRu", m_bLangRu);
    m_settings.setValue("/settings/customScreenH", customScreenH);
    _dbg_end(__func__);
}

/* PUBLIC SLOTS */
void SimpleAsker::onAns() {
    _dbg_start(__func__);
    ExtQPushButton* pBtn = dynamic_cast<ExtQPushButton*>(sender());

    bool okay = false;

    for (auto v : rightAnswers) {
        if (v == pBtn->text())
            okay = true;
    }
    if (okay) {
        if (!q_ansType) {
            q_ansType = 1;
            ++q_rightAnsCnt;
        }
        pBtn->setStyleSheet("color:blue; background-color: rgba(50,255,50,100); font-weight:bold");
    } else {
        if (!q_ansType) {
            wrongAsks.push_back(currentAsk);
            q_ansType = -1;
        }
        pBtn->setStyleSheet("color:white; background-color: rgba(255,0,0,100)");
    }
    updateInfoLabel();
    _dbg_end(__func__);
}
void SimpleAsker::onAboutProgram() {
    _dbg_start(__func__);
    QString compilationTime = QString("%1 %2").arg(__DATE__).arg(__TIME__);
    QString txt = QString("<font>Версия: %1<br>"
                          "Дата и время сборки: %2<br>"
                          "Автор: Никита Гребенюк (@nikich340)<br></font>").arg(VERSION).arg(compilationTime);
    QDialog *pdlg = createDialog(txt, ":/common/nikich340.jpg", "OK", "-", true);
    pdlg->exec();
    pdlg->deleteLater();
    _dbg_end(__func__);
}
void SimpleAsker::onChooseQst() {
    _dbg_start(__func__);
    QPushButton* pBtn = dynamic_cast<QPushButton*>(sender());
    for (int i = 0; i < QSTnames.size(); ++i) {
        QString choppedName = pBtn->text();
        choppedName.chop(choppedName.size() - QSTnames[i].size());
        if (choppedName == QSTnames[i]) {
            choosedQst = i;
            return;
        }
    }
    _dbg_end(__func__);
}
void SimpleAsker::onFinishAsk() {
    _dbg_start(__func__);
    if (q_cnt < 1)
        q_cnt = 1;
    qreal score = (qreal) q_rightAnsCnt / (qreal) q_cnt;
    QString info = "Ваш результат: " + to_str(q_rightAnsCnt) + "/" + to_str(q_cnt) + " = " + to_str(score * 100.0) + "%";
    QString pix = ":/common/score/2.jpg";

    m_pLblPix->setVisible(!isResolving && q_cnt >= 5);
    if (score >= 0.9) {
            pix = ":/common/score/5.jpg";
            m_pPlayer->setMedia(QUrl("qrc:/common/score/5.mp3"));
    } else if (score >= 0.7) {
            pix = ":/common/score/4.jpg";
            m_pPlayer->setMedia(QUrl("qrc:/common/score/4.mp3"));
    } else if (score >= 0.5) {
            pix = ":/common/score/3.jpg";
            m_pPlayer->setMedia(QUrl("qrc:/common/score/3.mp3"));
    } else {
            pix = ":/common/score/2.jpg";
            m_pPlayer->setMedia(QUrl("qrc:/common/score/2.mp3"));
        }
    if (m_pPlayer && m_pPlayer->state() == QMediaPlayer::PlayingState)
        m_pPlayer->stop();
    if (q_cnt >= 5 && !isResolving && !m_pPlayer->media().isNull()) {
            m_pPlayer->play();
    }

    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,120)");
    }
    q_sum = q_rightAnsCnt = q_ansType = 0;
    q_cnt = 1;

    m_pResolve->setEnabled(!wrongAsks.empty());
    m_plblInfo->setText("<h2>" + info + "</h2>");
    m_pLblPix->adjustSize();
    m_pLblPix->setPixmap(QPixmap(pix).scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setCurrentWidget(m_pWidgetWrong);
    _dbg_end(__func__);
}
void SimpleAsker::onFinishOsteoAsk() {
    _dbg_start(__func__);
    disconnect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    disconnect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    disconnect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    onFinishAsk();
    _dbg_end(__func__);
}
void SimpleAsker::onMenu() {
    _dbg_start(__func__);


    setCurrentWidget(m_pWidgetMenu);

    _dbg_end(__func__);
}
void SimpleAsker::onNextOsteoAsk() {
    _dbg_start(__func__);
    q_ansType = 0;
    upn(i, 0, maxAns - 1) {
        m_pBtnAns[i]->setStyleSheet("background-color: rgba(255,255,255,120)");
    }
    genQuest();
    ++q_cnt;
    if (q_cnt == q_sum) {
        m_pBtnNext->setEnabled(false);
    }
    updateInfoLabel();
    _dbg_end(__func__);
}
void SimpleAsker::onPreStartOsteoAsk() {
    _dbg_start(__func__);

    setCurrentWidget(m_pWidgetPreAsk);
    isResolving = false;

    QDialog* pdlg = new QDialog(this);
    QVBoxLayout* pvbox = new QVBoxLayout;
    QLabel* plbl = new QLabel("<h3>Выберите нужный раздел теста</h3>");
    plbl->setAlignment(Qt::AlignCenter);
    plbl->setWordWrap(true);
    pvbox->addWidget(plbl);
    for (int i = 0; i < QSTnames.size(); ++i) {
        QString name = QSTnames[i];
        name += " [" + to_str(QSTs[i].size()) + "]";
        QPushButton* btn = new QPushButton(name);
        btn->setStyleSheet(QString("text-align: top left; color:white; background-color: rgba(0,100,0,175); min-height: %1px").arg(30 * densityK));
        connect(btn, SIGNAL(clicked(bool)), pdlg, SLOT(accept()));
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(onChooseQst()));
        pvbox->addWidget(btn);
    }
    pdlg->setModal(true);
    pdlg->setLayout(pvbox);
    pdlg->setMaximumWidth(screenW);
    pdlg->exec();

    q_unusedAsks.clear();
    q_unusedAsks = QSTs[choosedQst];
    m_pLineStartFrom->setValidator(new QIntValidator(1, q_unusedAsks.size()));

    connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    _dbg_end(__func__);
}
void SimpleAsker::onResolve() {
    _dbg_start(__func__);
    q_unusedAsks.clear();
    q_unusedAsks = wrongAsks;
    wrongAsks.clear();

    q_sum = q_unusedAsks.size();
    genQuest();
    connect(m_pBtnStart, SIGNAL(clicked(bool)), this, SLOT(onStartOsteoAsk()));
    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    isResolving = true;
    onStartAsk();
    _dbg_end(__func__);
}
void SimpleAsker::onSettings() {
    _dbg_start(__func__);
    m_pCheckRus->setCheckState(m_bLangRu ? Qt::Checked : Qt::Unchecked);

    m_pDialogSettings->exec();
    _dbg_end(__func__);
}
void SimpleAsker::onSetStyleSheets(int setCustomScreenH) {
    _dbg_start(__func__);
    customScreenH = setCustomScreenH;
    densityK = (qreal) customScreenH / 1000.0;
    QString sheetStr = QString("QPushButton { spacing: 2; alignment: center; text-align: center; min-height: %1px; font-size: %2px; "
                               "background-color: rgba(255,255,255,160) }").arg((int)(80 * densityK)).arg((int)(22 * densityK)) +
                       QString("QLabel { spacing: 2; alignment: center; text-align: center; font-size: %1px; "
                               "background-color: rgba(255,255,255,160) }").arg((int)(22 * densityK)) +
                       QString("QTreeWidget { alignment: center; text-align: center; font-size: %1px; "
                               "background-color: rgba(255,255,255,200) }").arg((int)(25 * densityK)) +
                       QString("QLineEdit { alignment: center; text-align: center; min-height: %1px; font-size: %2px; "
                               "background-color: rgba(255, 255, 255, 160) }").arg((int)(75 * densityK)).arg((int)(25 * densityK)) +
                       QString("QCheckBox { alignment: center; text-align: center; font-size: %1px }").arg((int)(25 * densityK));
    this->setStyleSheet(sheetStr);
    m_pBtnNext->setStyleSheet(QString("color:white; background-color: rgba(0,0,255,175); min-height: %1px").arg((int)(50 * densityK)));
    m_pBtnFinish->setStyleSheet(QString("color:white; background-color: rgba(0,0,0,175); min-height: %1px").arg((int)(50 * densityK)));
    m_pLblQuestion->setStyleSheet(QString("font-size: %1px; font-weight:bold; color: #001a00").arg((int)(25 * densityK)));
    m_pLblInfo->setStyleSheet(QString("font-size: %1px; color: #990099").arg((int)(22 * densityK)));
    m_pDensitySlider->setMinimumHeight(22 * densityK);
    _dbg_end(__func__);
}
void SimpleAsker::onStartAsk() {
    _dbg_start(__func__);

    updateInfoLabel();
    m_pBtnNext->setEnabled(true);
    setCurrentWidget(m_pWidgetAsk);

    _dbg_end(__func__);
}
void SimpleAsker::onStartOsteoAsk() {
    _dbg_start(__func__);

    int start = m_pLineStartFrom->text().toInt() - 1;
    upn(i, 0, start - 1) {
        q_unusedAsks.erase(q_unusedAsks.begin());
    }
    q_sum = q_unusedAsks.size();
    wrongAsks.clear();
    genQuest();

    connect(m_pBtnFinish, SIGNAL(clicked(bool)), this, SLOT(onFinishOsteoAsk()));
    connect(m_pBtnNext, SIGNAL(clicked(bool)), this, SLOT(onNextOsteoAsk()));
    _dbg_end(__func__);
    onStartAsk();
}
void SimpleAsker::onUpdateLanguage(int check) {
    _dbg_start(__func__);
    if (check == Qt::Checked) {
        m_bLangRu = true;
    } else {
        m_bLangRu = false;
    }
    m_pToMenu->setText("Меню");
    m_pExit->setText("Выход");
    m_pResolve->setText("Перерешать неправильные");
    m_pBtnMenu[0]->setText(m_bLangRu ? "Предэкз наборы" : "Preex sets");
    m_pBtnMenu[1]->setText(m_bLangRu ? "Настройки" : "Settings");
    m_pBtnMenu[2]->setText(m_bLangRu ? "О программе" : "About program");
    m_pBtnMenu[3]->setText(m_bLangRu ? "Выход" : "Quit");
    m_pBtnStart->setText(m_bLangRu ? "Начать" : "Start");
    m_pBtnNext->setText(m_bLangRu ? "Далее" : "Next");
    m_pBtnFinish->setText(m_bLangRu ? " Завершить" : " Finish");
    m_pCheckRus->setText(m_bLangRu ? "Русский язык" : "Russian language");
    m_pCheckRandomize->setText(m_bLangRu ? "Случайный порядок вопросов" : "Random order of questions");
    m_pLblStartFrom->setText(m_bLangRu ? "Начинать с вопроса: " : "Start from question: ");
    _dbg_end(__func__);
}
