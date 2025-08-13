# kadistudio

**kadistudio** is a graphical desktop application designed to complement and extend the [Kadi4Mat](https://kadi.iam.kit.edu/) research data infrastructure for materials science. Developed at the Karlsruhe Institute of Technology (KIT), kadistudio provides a native, interactive interface for managing, visualizing, and analyzing materials data in conjunction with Kadi4Mat.

## About Kadi4Mat

[Kadi4Mat](https://kadi.iam.kit.edu/) (Karlsruhe Data Infrastructure for Materials Science) is a platform that supports the FAIR principles (Findable, Accessible, Interoperable, Reusable) for research data management. It enables structured storage, sharing, and reuse of data across the materials science community and beyond.

While Kadi4Mat offers a powerful web-based interface and backend, **kadistudio** enhances the user experience by offering:

- A native desktop GUI built with Qt
- Rich data visualization and exploration tools
- Plugin support for custom workflows
- Integration with local and remote Kadi4Mat instances

## Plugin System

kadistudio supports a modular plugin architecture that allows users to extend its functionality without modifying the core application.
Plugins can be developed in C++ and are loaded dynamically at runtime.
Plugins can be used to add new tools, automate workflows, or integrate external software and data sources.

### Plugin Structure

A plugin typically consists of:

- A class that inherits from the appropriate plugin interface (`PluginInterface` in C++).
- A `run()` method that defines the plugin's behavior.
- A set of metadata that describes the plugin's name, version, and dependencies.

### Example Plugin Use Cases (not yet all public available)

- **Data Import/Export Plugins**
  Import experimental data from CSV, Excel, or instrument-specific formats and export processed data to Kadi4Mat or local files.

- **Visualization Plugins**
  Create custom plots (e.g., stress-strain curves, phase diagrams) using libraries like matplotlib or QCustomPlot.

- **Simulation Integration**
  Launch external simulation tools (e.g., Phasefield) and automatically link results to Kadi4Mat records.

- **Metadata Enrichment**
  Automatically extract and suggest metadata from uploaded files using NLP or image analysis.

- **Batch Processing**
  Apply transformations or analyses to multiple datasets at once, such as normalization, filtering, or statistical evaluation.

## Getting Started

### Prerequisites

    CMake (≥ 3.16)
    C++20 compatible compiler
    Qt 6.2+
    Git

### Build Instructions

  - Clone the repository:

    ```
    git clone --recursive https://github.com/kadi4mat/kadi-studio.git
    cd kadi-studio
    ```

  - Create a build directory:

    ```
    mkdir build
    cd build
    ```

  - Configure the project with CMake:

    ```
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

    If Qt is not in your default path, specify it manually:

    ```
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt
    ```

  - Build the project:

    ```
    cmake --build .
    ```

  - Run kadistudio:

    ```
    ./bin/kadistudio
    ```

## Developing Plugins

  More info to come

## Community & Support

- Open an issue with your question.
- Join our community discussions on [Mattermost Community Channel](https://mattermost.hzdr.de/kadi4mat-community)
- Use email [feedback-kadi4mat@lists.kit.edu](mailto:feedback-kadi4mat@lists.kit.edu)
- Reach out to maintainers via GitLab.

## License

This project is licensed under the Apache License 2.0. See the LICENSE file for details.
