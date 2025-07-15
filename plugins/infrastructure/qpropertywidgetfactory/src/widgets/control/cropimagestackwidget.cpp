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

#include <QtWidgets/QGraphicsView>
#include <QDebug>
#include <QtCore/QDir>
#include <QRegularExpression>
#include "internal/cropimagewidget.h"

#include "cropimagestackwidget.h"

CropImageStackWidget::CropImageStackWidget(Property* property, QWidget* parent)
    : QPropertyWidget(property, new QWidget(parent)) {

  layout = new QVBoxLayout(getWidget());
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  imageView = new CropImageWidget();
  imageView->setAlignment(Qt::AlignCenter);
  imageView->setText("image not found");

  // connect mouse events for rectangle selection
  connect(imageView, &CropImageWidget::selectionChanged, this, &CropImageStackWidget::onSelectionChanged);
  connect(imageView, &CropImageWidget::selectionRemoved, this, &CropImageStackWidget::onSelectionRemoved);

  layout->addWidget(imageView);
  layout->setAlignment(imageView, Qt::AlignHCenter);
  selectionInfoLabel = new QLabel("No selection");
  selectionInfoLabel->setAlignment(Qt::AlignCenter);
  selectionInfoLabel->setStyleSheet("background-color: lightgrey;");
  layout->addWidget(selectionInfoLabel);
  layout->addSpacing(10);
  imageSlider = new QSlider();
  imageSlider->setOrientation(Qt::Orientation::Horizontal);
  connect(imageSlider, &QSlider::valueChanged, this, &CropImageStackWidget::onSliderValueChanged);
  layout->addWidget(imageSlider);
}

void CropImageStackWidget::synchronizeVTI() {
  long x, y, width, height;
  auto ambassador = getInnerAmbassador();

  std::string imagepath = ambassador->getValue<std::string>("imagePath");
  setImageStackPath(QString::fromStdString(imagepath));

  ambassador->getValue("x", x);
  ambassador->getValue("y", y);
  ambassador->getValue("width", width);
  ambassador->getValue("height", height);
  imageView->setSelection(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height));
}

bool CropImageStackWidget::setImageStackPath(const QString &value) {
  bool success = false;
  imageStackPath = value;

  // replace the first occurrence of ~ in imageStackPath by the users home folder
  QString toReplace = "~";
  int pos = imageStackPath.indexOf(toReplace);
  if (pos == 0) {
    imageStackPath.replace(pos, toReplace.size(), QDir::homePath());
  }

  QRegularExpression numbersEndExp("([0-9]+)\\.[a-z0-9]*$"); // matches any filename which contains numbers at the end, e.g. img_000.tif
  QRegularExpression regExp(IMAGE_PATH_REGEX);
  QFileInfo fileInfo(QFile(imageStackPath).fileName());
  QString fileName(fileInfo.fileName());
  QRegularExpressionMatch numbersMatch = numbersEndExp.match(fileName);
  int numbers_pos = numbersMatch.capturedStart(); // position of relevant numbers

  QRegularExpressionMatch match = regExp.match(imageStackPath);
  if (match.hasMatch()) {
    success = setImageStackWithPattern(value);
  } else if (numbers_pos != -1) {
    // the path did not contain a pattern, but the fileName ends with a block of numbers -> replace them accordingly
    // e.g. ".../000_img_z000.tif"     -> ".../000_img_z%03ld.tif"
    //      "path_123/example1240.png" -> "path_123/example%04ld.png"

    QRegularExpression numbersExp("[0-9]+");
    QString numbersEnd =
      fileName.right(fileName.length() - numbers_pos); // this is the part of the fileName starting from the numbers, e.g. 0123.png
    QRegularExpressionMatch matchNumbers = numbersExp.match(numbersEnd);
    QString digitsStr = "";
    if (match.hasMatch()) {
      digitsStr = match.captured(0);
      numbers_pos = match.capturedStart();
    }
    QString pattern = QString("%%1ld").arg(digitsStr.length(), 2, 10, QChar('0')); // 2 is the field with (to get %03ld instead of %3ld)
    QString numbersEndPatternized = numbersEnd.replace(numbersExp, pattern);

    // use numbersEndExp here to assure that only the relevant block of digits is replaced
    imageStackPath = imageStackPath.replace(numbersEndExp, numbersEndPatternized);

    // - in case the replace was unsuccessful, the following call will return false
    // - in case the replacement was successful but there was only one image, then the stack contains only one image
    success = setImageStackWithPattern(imageStackPath);

  } else {
    // no replacable pattern for numbers found in the path -> set the image and disable the slider
    QFileInfo check_file(value);
    if (check_file.exists() && check_file.isFile()) {
      setImage(value);
      imageSlider->setDisabled(true);
      success = true;
    }
  }
  if (!success) {
    // no image could be loaded
    imageStackPath.clear();

    imageView->setFixedSize(200, 200);
    getWidget()->setFixedSize(200, 250);
    imageSlider->setDisabled(true);
    imageView->setDisabled(true);
  }
  return success;
}

void CropImageStackWidget::scanImages() {
  QRegularExpression regExp(IMAGE_PATH_REGEX);
  QFileInfo pathInfo(imageStackPath);
  QDir dir = pathInfo.absoluteDir();

  QString fileName = pathInfo.fileName();
  QString pattern = fileName.replace(regExp, "*");

  fileName = pathInfo.fileName();
  QString regexPattern = "^" + fileName.replace(regExp, QString("[0-9]{%1}").arg(numberOfDigits)) + "$";
  QRegularExpression numberedFileNameExp(regexPattern);

  // sorted files in directory
  auto fileList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::LocaleAware);

  for (const auto &file : fileList) {
    const QFileInfo& fileInfo(file);
    if (fileInfo.isFile()) {
      bool match = numberedFileNameExp.match(fileInfo.fileName()).hasMatch();
      if (match) {
        imageFileNames.push_back(fileInfo.absoluteFilePath());
      }
    }
  }

}

void CropImageStackWidget::onSliderValueChanged(int newValue) {
  if (newValue >= 0 && newValue < (int) imageFileNames.size() && !imageFileNames[newValue].isEmpty()) {
    setImage(imageFileNames[newValue]);
  }
}

void CropImageStackWidget::setImage(QString url) {
  imageView->setUrl(url);

  QImage image(url);
  imageView->setPixmap(QPixmap::fromImage(image)); // this only works here, but should ideally be done inside imageView
                                                   // (using a interface function imageView->setImage(...))
  imageView->setFixedSize(image.size());
  layout->setSizeConstraint(QLayout::SetFixedSize);

  emit imageChanged(url);
}

void CropImageStackWidget::onSelectionChanged(QRect newSelection) {
  selectionInfoLabel->setText("Selection: " + QString::number(newSelection.width()) + "x"
    + QString::number(newSelection.height()));

  auto ambassador = getInnerAmbassador();
  auto suspend = ambassador->suspend();

  ambassador->setValue("x", (long) newSelection.x());
  ambassador->setValue("y", (long) newSelection.y());
  ambassador->setValue("width", (long) newSelection.width());
  ambassador->setValue("height", (long) newSelection.height());
}

void CropImageStackWidget::onSelectionRemoved() {
  selectionInfoLabel->setText("No selection");

  auto ambassador = getInnerAmbassador();
  auto suspend = ambassador->suspend();

  // set property values to -1 (not set)
  ambassador->setValue("x", -1L);
  ambassador->setValue("y", -1L);
  ambassador->setValue("width", -1L);
  ambassador->setValue("height", -1L);
}

QString CropImageStackWidget::selectionToString(const QRect &rect) {
  return QString("[%1, %2, %3, %4]").arg(QString::number(rect.x()), QString::number(rect.y()),
                                         QString::number(rect.width()), QString::number(rect.height()));
}

bool CropImageStackWidget::setImageStackWithPattern(const QString &path) {
  bool success = false;
  QRegularExpression regExp(IMAGE_PATH_REGEX);
  QRegularExpressionMatch matchRegExp = regExp.match(path);
  QString captured = "";
  if (matchRegExp.hasMatch()) {
    QString captured = matchRegExp.captured(1); // select first occurrence
  }
  numberOfDigits = captured.toInt(&success);
  imageSlider->setMinimum(0);

  scanImages();
  int numberOfImages = imageFileNames.size();
  if (success && numberOfImages > 0) {
    imageSlider->setMaximum(numberOfImages - 1);
    imageSlider->setValue(0);

    setImage(imageFileNames[0]);
    success = true;
  }
  return success;
}
