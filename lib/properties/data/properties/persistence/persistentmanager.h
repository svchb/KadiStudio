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

class Property;


/**
 * @brief      provides an interface to load and save
 *             Properties data to disk
 * @ingroup    persistence
 */
class PersistentManager {
  public:
    /* ------------------------- API FOR SAVING ---------------------------*/
    virtual ~PersistentManager() {}

    /**
      * @brief Open a file to the save properties and other values
      *
      * @param pathToFile     The path under which the file should be stored
      *
      * @return               True if the file was opened successfully
      */
    virtual bool openFileToSave(const std::string& pathToFile) = 0;

    /**
      * @brief Close a file to the save properties and other values
      */
    virtual void closeFileToSave(const std::string& pathToFile) = 0;

    virtual void storeProperty(const Property* property, const std::string& keynamespace) = 0;

    /* ------------------------- API FOR LOADING ---------------------------*/

    /**
      * @brief  prepares the internal structures and initialises them with the content of the passed file
      *
      * @param  pathToFile  name of the file to open
      *
      * @note   This method has to be invoked before any loading can be performed
      */
    virtual bool loadFile(const std::string& pathToFile) = 0;

    virtual void loadProperty(Property* property, const std::string& keynamespace) = 0;

    /**
      * @brief removes a stored session
      *
      * @param keynamespace The name of the session to be removed
      * @param pathToFile  The path to the file which holds the data
      */
    virtual bool removeNamespace(const std::string& keynamespace, const std::string& pathToFile) = 0;
};
