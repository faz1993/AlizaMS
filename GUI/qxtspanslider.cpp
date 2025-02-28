/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtGui module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/
#include "qxtspanslider.h"
#include "qxtspanslider_p.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionSlider>

QxtSpanSliderPrivate::QxtSpanSliderPrivate()
    :
    lower(0),
    upper(0),
    lowerPos(0),
    upperPos(0),
    offset(0),
    position(0),
    lastPressed(NoHandle),
    mainControl(LowerHandle),
    lowerPressed(QStyle::SC_None),
    upperPressed(QStyle::SC_None),
    movement(QxtSpanSlider::FreeMovement),
    firstMovement(false),
    blockTracking(false)
{
}

void QxtSpanSliderPrivate::initStyleOption(QStyleOptionSlider* option, SpanHandle handle) const
{
    if (!option) return;
    const QSlider* p = &qxt_p();
    option->initFrom(p);
    option->subControls = QStyle::SC_None;
    option->activeSubControls = QStyle::SC_None;
    option->orientation = p->orientation();
    option->maximum = p->maximum();
    option->minimum = p->minimum();
    option->tickPosition = p->tickPosition();
    option->tickInterval = p->tickInterval();
    option->upsideDown = (p->orientation() == Qt::Horizontal) ?
                         (p->invertedAppearance() != (option->direction == Qt::RightToLeft)) : (!p->invertedAppearance());
    option->direction = Qt::LeftToRight; // upsideDown instead
    option->sliderPosition = (handle == LowerHandle ? lowerPos : upperPos);
    option->sliderValue = (handle == LowerHandle ? lower : upper);
    option->singleStep = p->singleStep();
    option->pageStep = p->pageStep();
    if (p->orientation() == Qt::Horizontal)
    {
        option->state |= QStyle::State_Horizontal;
    }
}

int QxtSpanSliderPrivate::pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    int sliderMin = 0;
    int sliderMax = 0;
    int sliderLength = 0;
    const QSlider* p = &qxt_p();
    const QRect gr = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, p);
    const QRect sr = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, p);
    if (p->orientation() == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return (QStyle::sliderValueFromPosition(
                p->minimum(),
                p->maximum(),
                pos - sliderMin,
                sliderMax - sliderMin,
                opt.upsideDown));
}

void QxtSpanSliderPrivate::handleMousePress(const QPoint& pos, QStyle::SubControl& control, int value, SpanHandle handle)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, handle);
    QSlider* p = &qxt_p();
    const QStyle::SubControl oldControl = control;
    control = p->style()->hitTestComplexControl(QStyle::CC_Slider, &opt, pos, p);
    const QRect sr = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, p);
    if (control == QStyle::SC_SliderHandle)
    {
        position = value;
        offset = pick(pos - sr.topLeft());
        lastPressed = handle;
        p->setSliderDown(true);
    }
    if (control != oldControl) p->update(sr);
}

void QxtSpanSliderPrivate::setupPainter(QPainter* painter, Qt::Orientation orientation, qreal x1, qreal y1, qreal x2, qreal y2) const
{
    QColor highlight = qxt_p().palette().color(QPalette::Highlight);
    QLinearGradient gradient(x1, y1, x2, y2);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
    gradient.setColorAt(0, highlight.darker(120));
    gradient.setColorAt(1, highlight.lighter(108));
#else
    gradient.setColorAt(0, highlight.dark(120));
    gradient.setColorAt(1, highlight.light(108));
#endif
    painter->setBrush(gradient);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
    if (orientation == Qt::Horizontal)
    {
        painter->setPen(QPen(highlight.darker(130), 0));
    }
    else
    {
        painter->setPen(QPen(highlight.darker(150), 0));
    }
#else
    if (orientation == Qt::Horizontal)
    {
        painter->setPen(QPen(highlight.dark(130), 0));
    }
    else
    {
        painter->setPen(QPen(highlight.dark(150), 0));
    }
#endif
}

void QxtSpanSliderPrivate::drawSpan(QStylePainter* painter, const QRect& rect) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    const QSlider* p = &qxt_p();
    // area
    QRect groove = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, p);
    if (opt.orientation == Qt::Horizontal) groove.adjust(0, 0, -1, 0);
    else groove.adjust(0, 0, 0, -1);
    // pen, brush
    painter->setPen(QPen(p->palette().color(QPalette::Dark), 0));
    if (opt.orientation == Qt::Horizontal)
    {
        setupPainter(painter, opt.orientation, groove.center().x(), groove.top(), groove.center().x(), groove.bottom());
    }
    else
    {
        setupPainter(painter, opt.orientation, groove.left(), groove.center().y(), groove.right(), groove.center().y());
    }
    painter->drawRect(rect.intersected(groove));
}

void QxtSpanSliderPrivate::drawHandle(QStylePainter* painter, SpanHandle handle) const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt, handle);
    opt.subControls = QStyle::SC_SliderHandle;
    QStyle::SubControl pressed = (handle == LowerHandle ? lowerPressed : upperPressed);
    if (pressed == QStyle::SC_SliderHandle)
    {
        opt.activeSubControls = pressed;
        opt.state |= QStyle::State_Sunken;
    }
    painter->drawComplexControl(QStyle::CC_Slider, opt);
}

void QxtSpanSliderPrivate::triggerAction(QAbstractSlider::SliderAction action, bool main)
{
    int value = 0;
    bool no = false;
    bool up = false;
    const int min = qxt_p().minimum();
    const int max = qxt_p().maximum();
    const SpanHandle altControl = (mainControl == LowerHandle ? UpperHandle : LowerHandle);
    blockTracking = true;
    switch (action)
    {
    case QAbstractSlider::SliderSingleStepAdd:
        if ((main && mainControl == UpperHandle) || (!main && altControl == UpperHandle))
        {
            value = qBound(min, upper + qxt_p().singleStep(), max);
            up = true;
            break;
        }
        value = qBound(min, lower + qxt_p().singleStep(), max);
        break;
    case QAbstractSlider::SliderSingleStepSub:
        if ((main && mainControl == UpperHandle) || (!main && altControl == UpperHandle))
        {
            value = qBound(min, upper - qxt_p().singleStep(), max);
            up = true;
            break;
        }
        value = qBound(min, lower - qxt_p().singleStep(), max);
        break;
    case QAbstractSlider::SliderToMinimum:
        value = min;
        if ((main && mainControl == UpperHandle) || (!main && altControl == UpperHandle))
            up = true;
        break;
    case QAbstractSlider::SliderToMaximum:
        value = max;
        if ((main && mainControl == UpperHandle) || (!main && altControl == UpperHandle))
            up = true;
        break;
    case QAbstractSlider::SliderMove:
    case QAbstractSlider::SliderNoAction:
        if ((action == QAbstractSlider::SliderMove) &&
            ((main && mainControl == UpperHandle) || (!main && altControl == UpperHandle)))
        {
            up = true;
        }
        no = true;
        break;
    default:
        qWarning("QxtSpanSliderPrivate::triggerAction: Unknown action");
        break;
    }
    if (!no && !up)
    {
        if (movement == QxtSpanSlider::NoCrossing)
        {
            value = qMin(value, upper);
        }
        else if (movement == QxtSpanSlider::NoOverlapping)
        {
            value = qMin(value, upper - 1);
        }
        if (movement == QxtSpanSlider::FreeMovement && value > upper)
        {
            swapControls();
            qxt_p().setUpperPosition(value);
        }
        else
        {
            qxt_p().setLowerPosition(value);
        }
    }
    else if (!no)
    {
        if (movement == QxtSpanSlider::NoCrossing)
        {
            value = qMax(value, lower);
        }
        else if (movement == QxtSpanSlider::NoOverlapping)
        {
            value = qMax(value, lower + 1);
        }
        if (movement == QxtSpanSlider::FreeMovement && value < lower)
        {
            swapControls();
            qxt_p().setLowerPosition(value);
        }
        else
        {
            qxt_p().setUpperPosition(value);
        }
    }
    blockTracking = false;
    qxt_p().setLowerValue(lowerPos);
    qxt_p().setUpperValue(upperPos);
}

void QxtSpanSliderPrivate::swapControls()
{
    qSwap(lower, upper);
    qSwap(lowerPressed, upperPressed);
    lastPressed = (lastPressed == LowerHandle ? UpperHandle : LowerHandle);
    mainControl = (mainControl == LowerHandle ? UpperHandle : LowerHandle);
}

void QxtSpanSliderPrivate::updateRange(int min, int max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    qxt_p().setSpan(lower, upper);
}

void QxtSpanSliderPrivate::movePressedHandle()
{
    switch (lastPressed)
    {
        case QxtSpanSliderPrivate::LowerHandle:
            if (lowerPos != lower)
            {
                bool main = (mainControl == QxtSpanSliderPrivate::LowerHandle);
                triggerAction(QAbstractSlider::SliderMove, main);
            }
            break;
        case QxtSpanSliderPrivate::UpperHandle:
            if (upperPos != upper)
            {
                bool main = (mainControl == QxtSpanSliderPrivate::UpperHandle);
                triggerAction(QAbstractSlider::SliderMove, main);
            }
            break;
        default:
            break;
    }
}

QxtSpanSlider::QxtSpanSlider(QWidget* parent) : QSlider(parent)
{
    qxt_d.setPublic(this);
    connect(this, SIGNAL(rangeChanged(int, int)), &qxt_d(), SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), &qxt_d(), SLOT(movePressedHandle()));
}

QxtSpanSlider::QxtSpanSlider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent)
{
    qxt_d.setPublic(this);
    connect(this, SIGNAL(rangeChanged(int, int)), &qxt_d(), SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), &qxt_d(), SLOT(movePressedHandle()));
}

QxtSpanSlider::~QxtSpanSlider()
{
}

QxtSpanSlider::HandleMovementMode QxtSpanSlider::handleMovementMode() const
{
    return qxt_d().movement;
}

void QxtSpanSlider::setHandleMovementMode(QxtSpanSlider::HandleMovementMode mode)
{
    qxt_d().movement = mode;
}

int QxtSpanSlider::lowerValue() const
{
    return qMin(qxt_d().lower, qxt_d().upper);
}

void QxtSpanSlider::setLowerValue(int lower)
{
    setSpan(lower, qxt_d().upper);
}

int QxtSpanSlider::upperValue() const
{
    return qMax(qxt_d().lower, qxt_d().upper);
}

void QxtSpanSlider::setUpperValue(int upper)
{
    setSpan(qxt_d().lower, upper);
}

void QxtSpanSlider::setSpan(int lower, int upper)
{
    const int low = qBound(minimum(), qMin(lower, upper), maximum());
    const int upp = qBound(minimum(), qMax(lower, upper), maximum());
    if (low != qxt_d().lower || upp != qxt_d().upper)
    {
        if (low != qxt_d().lower)
        {
            qxt_d().lower = low;
            qxt_d().lowerPos = low;
            emit lowerValueChanged(low);
        }
        if (upp != qxt_d().upper)
        {
            qxt_d().upper = upp;
            qxt_d().upperPos = upp;
            emit upperValueChanged(upp);
        }
        emit spanChanged(qxt_d().lower, qxt_d().upper);
        update();
    }
}

int QxtSpanSlider::lowerPosition() const
{
    return qxt_d().lowerPos;
}

void QxtSpanSlider::setLowerPosition(int lower)
{
    if (qxt_d().lowerPos != lower)
    {
        qxt_d().lowerPos = lower;
        if (!hasTracking()) update();
        if (isSliderDown()) emit lowerPositionChanged(lower);
        if (hasTracking() && !qxt_d().blockTracking)
        {
            bool main = (qxt_d().mainControl == QxtSpanSliderPrivate::LowerHandle);
            qxt_d().triggerAction(SliderMove, main);
        }
    }
}

int QxtSpanSlider::upperPosition() const
{
    return qxt_d().upperPos;
}

void QxtSpanSlider::setUpperPosition(int upper)
{
    if (qxt_d().upperPos != upper)
    {
        qxt_d().upperPos = upper;
        if (!hasTracking()) update();
        if (isSliderDown()) emit upperPositionChanged(upper);
        if (hasTracking() && !qxt_d().blockTracking)
        {
            bool main = (qxt_d().mainControl == QxtSpanSliderPrivate::UpperHandle);
            qxt_d().triggerAction(SliderMove, main);
        }
    }
}

void QxtSpanSlider::keyPressEvent(QKeyEvent* event)
{
    QSlider::keyPressEvent(event);
    bool main = true;
    SliderAction action = SliderNoAction;
    switch (event->key())
    {
    case Qt::Key_Left:
        main   = (orientation() == Qt::Horizontal);
        action = !invertedAppearance() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;
    case Qt::Key_Right:
        main   = (orientation() == Qt::Horizontal);
        action = !invertedAppearance() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;
    case Qt::Key_Up:
        main   = (orientation() == Qt::Vertical);
        action = invertedControls() ? SliderSingleStepSub : SliderSingleStepAdd;
        break;
    case Qt::Key_Down:
        main   = (orientation() == Qt::Vertical);
        action = invertedControls() ? SliderSingleStepAdd : SliderSingleStepSub;
        break;
    case Qt::Key_Home:
        main   = (qxt_d().mainControl == QxtSpanSliderPrivate::LowerHandle);
        action = SliderToMinimum;
        break;
    case Qt::Key_End:
        main   = (qxt_d().mainControl == QxtSpanSliderPrivate::UpperHandle);
        action = SliderToMaximum;
        break;
    default:
        event->ignore();
        break;
    }
    if (action) qxt_d().triggerAction(action, main);
}

void QxtSpanSlider::mousePressEvent(QMouseEvent* event)
{
    if (minimum() == maximum() || (event->buttons() ^ event->button()))
    {
        event->ignore();
        return;
    }
    qxt_d().handleMousePress(event->pos(), qxt_d().upperPressed, qxt_d().upper, QxtSpanSliderPrivate::UpperHandle);
    if (qxt_d().upperPressed != QStyle::SC_SliderHandle)
        qxt_d().handleMousePress(event->pos(), qxt_d().lowerPressed, qxt_d().lower, QxtSpanSliderPrivate::LowerHandle);

    qxt_d().firstMovement = true;
    event->accept();
}

void QxtSpanSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (qxt_d().lowerPressed != QStyle::SC_SliderHandle && qxt_d().upperPressed != QStyle::SC_SliderHandle)
    {
        event->ignore();
        return;
    }
    QStyleOptionSlider opt;
    qxt_d().initStyleOption(&opt);
    const int m = style()->pixelMetric(QStyle::PM_MaximumDragDistance, &opt, this);
    int newPosition = qxt_d().pixelPosToRangeValue(qxt_d().pick(event->pos()) - qxt_d().offset);
    if (m >= 0)
    {
        const QRect r = rect().adjusted(-m, -m, m, m);
        if (!r.contains(event->pos()))
        {
            newPosition = qxt_d().position;
        }
    }
    if (qxt_d().firstMovement)
    {
        if (qxt_d().lower == qxt_d().upper)
        {
            if (newPosition < lowerValue())
            {
                qxt_d().swapControls();
                qxt_d().firstMovement = false;
            }
        }
        else
        {
            qxt_d().firstMovement = false;
        }
    }
    if (qxt_d().lowerPressed == QStyle::SC_SliderHandle)
    {
        if (qxt_d().movement == NoCrossing)
        {
            newPosition = qMin(newPosition, upperValue());
        }
        else if (qxt_d().movement == NoOverlapping)
        {
            newPosition = qMin(newPosition, upperValue() - 1);
        }
        if (qxt_d().movement == FreeMovement && newPosition > qxt_d().upper)
        {
            qxt_d().swapControls();
            setUpperPosition(newPosition);
        }
        else
        {
            setLowerPosition(newPosition);
        }
    }
    else if (qxt_d().upperPressed == QStyle::SC_SliderHandle)
    {
        if (qxt_d().movement == NoCrossing)
        {
            newPosition = qMax(newPosition, lowerValue());
        }
        else if (qxt_d().movement == NoOverlapping)
        {
            newPosition = qMax(newPosition, lowerValue() + 1);
        }
        if (qxt_d().movement == FreeMovement && newPosition < qxt_d().lower)
        {
            qxt_d().swapControls();
            setLowerPosition(newPosition);
        }
        else
        {
            setUpperPosition(newPosition);
        }
    }
    event->accept();
}

void QxtSpanSlider::mouseReleaseEvent(QMouseEvent* event)
{
    QSlider::mouseReleaseEvent(event);
    setSliderDown(false);
    qxt_d().lowerPressed = QStyle::SC_None;
    qxt_d().upperPressed = QStyle::SC_None;
    update();
}

void QxtSpanSlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStylePainter painter(this);
    QStyleOptionSlider opt;
    qxt_d().initStyleOption(&opt);
    opt.sliderValue = 0;
    opt.sliderPosition = 0;
    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderTickmarks;
    painter.drawComplexControl(QStyle::CC_Slider, opt);
    opt.sliderPosition = qxt_d().lowerPos;
    const QRect lr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int lrv  = qxt_d().pick(lr.center());
    opt.sliderPosition = qxt_d().upperPos;
    const QRect ur = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const int urv  = qxt_d().pick(ur.center());
    const int minv = qMin(lrv, urv);
    const int maxv = qMax(lrv, urv);
    const QPoint c = QRect(lr.center(), ur.center()).center();
    QRect spanRect;
    if (orientation() == Qt::Horizontal)
    {
        spanRect = QRect(QPoint(minv, c.y() - 2), QPoint(maxv, c.y() + 1));
    }
    else
    {
        spanRect = QRect(QPoint(c.x() - 2, minv), QPoint(c.x() + 1, maxv));
    }
    qxt_d().drawSpan(&painter, spanRect);
    switch (qxt_d().lastPressed)
    {
    case QxtSpanSliderPrivate::LowerHandle:
        qxt_d().drawHandle(&painter, QxtSpanSliderPrivate::UpperHandle);
        qxt_d().drawHandle(&painter, QxtSpanSliderPrivate::LowerHandle);
        break;
    case QxtSpanSliderPrivate::UpperHandle:
    default:
        qxt_d().drawHandle(&painter, QxtSpanSliderPrivate::LowerHandle);
        qxt_d().drawHandle(&painter, QxtSpanSliderPrivate::UpperHandle);
        break;
    }
}
