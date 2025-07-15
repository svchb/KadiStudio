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

#include <QJsonDocument>
#include <QJsonObject>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QtNodes/internal/NodeGraphicsObject.hpp>
#include <QFileDialog>
#include <QtSvg/QSvgGenerator>
#include <QtPrintSupport/QPrinter>

#include "WorkFlowGraphModel.h"

#include "nodes/models/toolnode.h"
#include <framework/pluginframework/plugininterface.h>
#include "framework/pluginframework/pluginmanager.h"
#include <plugins/infrastructure/dialogs/fileopen/fileopendialoginterface.h>
#include "workflowscene.h"

using QtNodes::NodeGraphicsObject;

WorkflowScene::WorkflowScene(WorkFlowGraphModel &graphModel, QObject *parent)
    : BasicGraphicsScene(graphModel, parent), ismodified(false) {
  graphModel.setScene(this);
  getWorkFlowGraphModel().initializeDefaultVariables();

  connect(&graphModel, &QtNodes::AbstractGraphModel::connectionCreated, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::connectionDeleted, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::nodeCreated, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::nodeDeleted, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::nodeUpdated, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::nodeFlagsUpdated, this, &WorkflowScene::modified);
  connect(&graphModel, &QtNodes::AbstractGraphModel::nodePositionUpdated, this, &WorkflowScene::modified);

  connect(&graphModel, &WorkFlowGraphModel::nodeCreated, this, &WorkflowScene::postNodeCreation);
}

void WorkflowScene::clearScene() {
  QtNodes::BasicGraphicsScene::clearScene();
  QtNodes::BasicGraphicsScene::undoStack().clear();
  getWorkFlowGraphModel().initializeDefaultVariables();

  workflowsavefile = "";

  this->ismodified = false;
  Q_EMIT saveFileSet(false);
  Q_EMIT filepathChanged("");
}

void WorkflowScene::selectAllNodes() {
  for (QGraphicsItem *item : items()) {
    item->setSelected(true);
  }
}

std::vector<NodeId> WorkflowScene::selectedNodes() const {
  QList<QGraphicsItem *> graphicsItems = selectedItems();

  std::vector<NodeId> result;
  result.reserve(graphicsItems.size());

  for (QGraphicsItem *item : graphicsItems) {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item);

    if (ngo != nullptr) {
      result.push_back(ngo->nodeId());
    }
  }

  return result;
}

void WorkflowScene::postNodeCreation(NodeId nodeId) {
  auto ngo = nodeGraphicsObject(nodeId);
  // Workaround for https://bugreports.qt.io/browse/QTBUG-77400
  ngo->setGraphicsEffect(nullptr);

  ToolNode *toolnode = dynamic_cast<ToolNode*>(getWorkFlowGraphModel().delegateModel<QtNodes::NodeDelegateModel>(nodeId));

  if (toolnode) {
    if (auto w = toolnode->cornerWidget()) {
      auto &geometry = nodeGeometry();
      auto _cornerProxyWidget = new QGraphicsProxyWidget(ngo);

      _cornerProxyWidget->setWidget(w);
      _cornerProxyWidget->setPreferredWidth(5);

      geometry.recomputeSize(nodeId);

      if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
      {
        // If the widget wants to use as much vertical space as possible, set it to have the geom's equivalentWidgetHeight.
        // _cornerProxyWidget->setMinimumHeight(geometry.equivalentCornerWidgetHeight());
      }

      int offsetx = _cornerProxyWidget->size().width()+7;
      int offsety = _cornerProxyWidget->size().height()-7;
      // reset width to fit in the corner widget
      QSize size = getWorkFlowGraphModel().nodeData(nodeId, NodeRole::Size).toSize();
      size.setWidth(size.width() + offsetx + 20);
      getWorkFlowGraphModel().setNodeData(nodeId, NodeRole::Size, QVariant(size));

      QPointF pos;
      pos.setX(geometry.widgetPosition(nodeId).x()+geometry.portPosition(nodeId, PortType::Out, 0).x()-offsetx);
      pos.setY(geometry.widgetPosition(nodeId).y()+offsety);

      _cornerProxyWidget->setPos(pos);

      update();

      _cornerProxyWidget->setOpacity(1.0);
      // _cornerProxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    }
  }
}

void WorkflowScene::modified() {
  this->ismodified = true;
  Q_EMIT saveFileSet(true);
}

void WorkflowScene::save(const QString& savefilepath) {
  QFile file(savefilepath);

  if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
    throw std::logic_error(tr("No write permission").toStdString());
  }

  QTextStream out(&file);
  out << QJsonDocument(getWorkFlowGraphModel().save()).toJson() << Qt::endl;
  file.close();

  ismodified = false;
  Q_EMIT saveFileSet(false);

  if (savefilepath != workflowsavefile) {
    workflowsavefile = savefilepath;

    Q_EMIT filepathChanged(savefilepath);
  }
}

void WorkflowScene::load(const QString& filepath) {
  clearScene();

  LibFramework::PluginManager *pluginmanager = LibFramework::PluginManager::getInstance();
  FileOpenDialogInterface *matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(pluginmanager, filepath);
  if (not matchingFileDialogInterface) {
    throw std::logic_error(tr("No valid file dialog found").toStdString());
  }
  QFile file;
  matchingFileDialogInterface->openFilePath(filepath, file);
  QByteArray wholeFile = file.readAll();
  file.close();

  getWorkFlowGraphModel().load(QJsonDocument::fromJson(wholeFile).object());

  ismodified = false;
  Q_EMIT saveFileSet(false);

  workflowsavefile = filepath;
  Q_EMIT filepathChanged(filepath);
}

void WorkflowScene::merge(const QString& filepath, const QPointF& offset) {
  clearSelection();

  LibFramework::PluginManager *pluginmanager = LibFramework::PluginManager::getInstance();
  FileOpenDialogInterface *matchingFileDialogInterface = FileOpenDialogInterface::getCompatibleFileOpenPlugin(pluginmanager, filepath);
  if (not matchingFileDialogInterface) {
    throw std::logic_error(tr("No valid file dialog found").toStdString());
  }
  QFile file;
  matchingFileDialogInterface->openFilePath(filepath, file);
  QByteArray wholeFile = file.readAll();
  file.close();
  QJsonObject mergescene = QJsonDocument::fromJson(wholeFile).object();
  getWorkFlowGraphModel().merge(mergescene, offset);

  modified();
}

void WorkflowScene::exportAsPng() {
  QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save Scene as image"),
                           QDir::homePath() + QDir::separator() + tr("untitled.jpg"),
                           tr("png (*.png);;bmp (*.bmp);;gif (*.gif);;jpeg (*.jpg *.jpeg);;tiff (*.tif *.tiff);;xpm (*.xpm);;All files (*.*)"));

  if (fileName.isEmpty()) return;

  QImage image(width(), height(), QImage::Format_ARGB32);
  image.fill(Qt::white);

  QPainter painter(&image);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  render(&painter);
  image.save(fileName);
}

void WorkflowScene::exportAsSvg() {
  QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save Scene as SVG"),
                           QDir::homePath() + QDir::separator() + tr("untitled.svg"),
                           tr("svg (*.svg);;pdf (*.pdf)"));

  if (fileName.isEmpty()) return;

  if (fileName.endsWith(".svg")) {
    QSvgGenerator svgGen;

    svgGen.setFileName( fileName );
    svgGen.setSize(QSize(width(), height()));
    svgGen.setViewBox(QRect(0, 0, width(), height()));
    svgGen.setTitle(tr("Workflow") + " " + workflowsavefile);
    svgGen.setDescription(tr("A workflow created with Workfloweditor of kadistudio."));

    QPainter painter( &svgGen );
    render( &painter );

  } else {
    QPrinter pdfPrinter;

    pdfPrinter.setOutputFileName( fileName );
    pdfPrinter.setOutputFormat( QPrinter::PdfFormat );
    pdfPrinter.setPageSize(QPageSize(QSize(width(), height())));
    pdfPrinter.setFullPage(true);

    QPainter pdfPainter;
    pdfPainter.begin( &pdfPrinter);
    render( &pdfPainter );
    pdfPainter.end();

  }
}
