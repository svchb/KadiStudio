/* Copyright 2025 Karlsruhe Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "userinteractioneventfilter.h"

#include <QWidget>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>

#include "src/qwidgetinterfaceimpl.h"
#include "src/widgets/qvtitooltip.h"

UserInteractionEventFilter::UserInteractionEventFilter(QObject* parent) : QObject(parent) {
}

bool UserInteractionEventFilter::eventFilter(QObject* obj, QEvent* event) {
  switch (event->type()) {
    case QEvent::ToolTip: {
      QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
      QList<QWidgetInterfaceImpl *> qwtilist = obj->findChildren<QWidgetInterfaceImpl*>();
      for (auto qwti : qwtilist) {
        if (qwti->getWidget()->underMouse()) {
          QVTIToolTip::showText(helpEvent->globalPos(), qwti);
          event->accept();
          return true;
        }
      }
    }
      break;
    // send signal to indicate focus in by user (e.g. to update description)
    case QEvent::FocusIn: {
      // QFocusEvent *focusevent = static_cast<QFocusEvent*>(event);
      // switch (focusevent->reason()) {
      //   case Qt::MouseFocusReason:
      //   case Qt::TabFocusReason:
      //   case Qt::BacktabFocusReason:
          Q_EMIT gotFocus();
      //   default:;
      // }
    }
      break;

    // send signal to indicate focus out by user (e.g. to reset description)
    case QEvent::FocusOut: {
      // QFocusEvent *focusevent = static_cast<QFocusEvent*>(event);
      // switch (focusevent->reason()) {
      //   case Qt::MouseFocusReason:
      //   case Qt::TabFocusReason:
      //   case Qt::BacktabFocusReason:
          Q_EMIT lostFocus();
      //   default:;
      // }
    }
      break;

    // catch wheel event so widgets will not be changed accidently
    case QEvent::Wheel:
      event->ignore();
      return true;

    // ignores modified alt arrow combination
    case QEvent::ShortcutOverride:
    case QEvent::KeyPress: {
      QKeyEvent *keyevent = static_cast<QKeyEvent*>(event);

      auto isUndo = keyevent->matches(QKeySequence::Undo);
      auto isRedo = keyevent->matches(QKeySequence::Redo);

      if (isUndo || isRedo) return true;
      [[fallthrough]];
    }

    default:;
  }
  // standard event processing
  return QObject::eventFilter(obj, event);
}
