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

#include <QString>


/**
 * @file       config.h
 * @brief      Configuration for the Workflow Editor
 * @ingroup    workfloweditor
 */

namespace EditorConfig {

  const QString GRAPHICS_VIEW_STYLE = R"(
  {
    "GraphicsViewStyle": {
      "BackgroundColor": [255, 255, 255],
      "FineGridColor": [245, 245, 230],
      "CoarseGridColor": [235, 235, 220]
    }
  }
  )";

  const QString NODE_STYLE = R"(
    {
      "NodeStyle": {
        "NormalBoundaryColor": "black",
        "SelectedBoundaryColor": [33, 116, 194],
        "GradientColor0": [245, 245, 245],
        "GradientColor1": [245, 245, 245],
        "GradientColor2": [245, 245, 245],
        "GradientColor3": [245, 245, 245],
        "FontColor": [10, 10, 10],
        "FontColorFaded": [100, 100, 100],
        "ConnectionPointColor": "white",
        "PenWidth": 2.0,
        "HoveredPenWidth": 2.5,
        "ConnectionPointDiameter": 10.0,
        "Opacity": 0.8,
        "WarningColor": [249, 224, 118]
      }
    }
  )";

  const QString TOOL_STYLE = R"(
  {
    "NodeStyle": {
      "NormalBoundaryColor": "black",
      "SelectedBoundaryColor": [33, 116, 194],
      "GradientColor0": [30, 50, 90],
      "GradientColor1": [30, 50, 90],
      "GradientColor2": [30, 50, 90],
      "GradientColor3": [30, 50, 90],
      "FontColor": [255, 255, 255],
      "FontColorFaded": [150, 150, 150],
      "ConnectionPointColor": "white",
      "PenWidth": 2.0,
      "HoveredPenWidth": 2.5,
      "ConnectionPointDiameter": 10.0,
      "Opacity": 0.8
    }
  }
  )";

  const QString ENV_STYLE = R"(
    {
      "NodeStyle": {
        "NormalBoundaryColor": "black",
        "SelectedBoundaryColor": [33, 116, 194],
        "GradientColor0": [13, 73, 33],
        "GradientColor1": [13, 73, 33],
        "GradientColor2": [13, 73, 33],
        "GradientColor3": [13, 73, 33],
        "FontColor": [255, 255, 255],
        "FontColorFaded": [150, 150, 150],
        "ConnectionPointColor": "white",
        "PenWidth": 2.0,
        "HoveredPenWidth": 2.5,
        "ConnectionPointDiameter": 10.0,
        "Opacity": 0.8
      }
    }
  )";

  const QString NOTE_STYLE = R"(
  {
    "NodeStyle": {
      "NormalBoundaryColor": [220, 220, 220],
      "SelectedBoundaryColor": [120, 120, 120],
      "GradientColor0": [255, 255, 224],
      "GradientColor1": [255, 255, 224],
      "GradientColor2": [255, 255, 224],
      "GradientColor3": [255, 255, 224],
      "FontColor": [10, 10, 10],
      "FontColorFaded": [100, 100, 100],
      "ConnectionPointColor": "white",
      "PenWidth": 2.0,
      "HoveredPenWidth": 2.5,
      "ConnectionPointDiameter": 10.0,
      "Opacity": 0.8
    }
  }
  )";

  const QString CONNECTION_STYLE = R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "white",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",
      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,
      "UseDataDefinedColors": true
    }
  }
  )";
}
