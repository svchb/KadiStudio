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

#pragma once

#include <QTabWidget>
#include <QEvent>

class QLabel;
class QTextEdit;
class ToolDescription;

/**
 * @brief      Constructs the InfoTabWidget of the ToolChooserWidget.
 * @ingroup    tooldialog
 */
class QTextEditEventFilter : public QObject {
    Q_OBJECT

  public:

    QTextEditEventFilter(QObject* parent) : QObject(parent) {};

    Q_SIGNALS:
      void gotFocus();
      void lostFocus();

  protected:

    virtual bool eventFilter(QObject* obj, QEvent* event) override {
      switch (event->type()) {
        // send signal to indicate focus in by user (e.g. to update description)
        case QEvent::FocusIn:
          Q_EMIT gotFocus();
          break;

        // send signal to indicate focus out by user (e.g. to reset description)
        case QEvent::FocusOut:
          Q_EMIT lostFocus();
          break;

        default:;
      }
      // standard event processing
      return QObject::eventFilter(obj, event);
    }

};

class InfoTabWidget : public QTabWidget {
    Q_OBJECT

  public:
    InfoTabWidget(QWidget *parent = nullptr);
    virtual ~InfoTabWidget();

    void clear();
    void setToolInformation(const ToolDescription* tooldescription);

  private:
    void showHelpText(const QString& description, const QString& example, bool flag);

    QLabel *toolnamelabel;
    QLabel *toolpathlabel;
    QLabel *toolversionlabel;
    QLabel *toolreleasedatelabel;
    QTextEdit *tooldescriptiontextedit;
    QTextEdit *toolexampleusagetextedit;

};
