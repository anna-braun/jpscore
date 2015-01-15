/**
 * \file        PedData.h
 * \date        Oct 10, 2014
 * \version     v0.6
 * \copyright   <2009-2014> Forschungszentrum J��lich GmbH. All rights reserved.
 *
 * \section License
 * This file is part of JuPedSim.
 *
 * JuPedSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * JuPedSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with JuPedSim. If not, see <http://www.gnu.org/licenses/>.
 *
 * \section Description
 * In this file functions related to method A are defined.
 *
 *
 **/

#ifndef PEDDATA_H_
#define PEDDATA_H_

#include <string>
#include <vector>
#include <map>
#include "general/Macros.h"
#include "tinyxml/tinyxml.h"
#include "IO/OutputHandler.h"
#include <algorithm>    // std::min_element, std::max_element
#include <boost/algorithm/string.hpp>
using namespace  std;
extern OutputHandler* Log;

#define M2CM 100
#define CMtoM 0.01

class PedData
{
public:
     PedData();
     virtual ~PedData();
     int GetMinFrame() const;
     int GetMinID() const;
     int GetNumFrames() const;
     int GetNumPeds() const;
     int GetFps() const;
     string GetTrajName() const;
     string GetProjectRootDir() const;
     map<int , vector<int>> GetPedsFrame() const;
     double** GetXCor() const;
     double** GetYCor() const;
     int* GetFirstFrame() const;
     int* GetLastFrame() const;
     vector<int> GetIdInFrame(const vector<int>& ids) const;
     vector<double> GetXInFrame(int frame, const vector<int>& ids) const;
     vector<double> GetYInFrame(int frame, const vector<int>& ids) const;
     vector<double> GetVInFrame(int frame, const vector<int>& ids) const;
     bool ReadData(const string& projectRootDir, const string& path, const string& filename, const FileFormat& _trajformat, int deltaF, char vComponent);


private:
     bool InitializeVariables(const string& filename);
     bool InitializeVariables(TiXmlElement* xRootNode);
     void CreateGlobalVariables(int numPeds, int numFrames);
     double GetInstantaneousVelocity(int Tnow,int Tpast, int Tfuture, int ID, int *Tfirst, int *Tlast, double **Xcor, double **Ycor) const;


private:

     string _trajName="";
     string _projectRootDir="";
     int _minFrame=0;
     int _minID=1;
     int _numFrames=0;  // total number of frames
     int _numPeds=0; // total number of pedestrians
     int _fps=16;
     map<int , vector<int>> _peds_t;

     int _deltaF=5;
     char _vComponent='X';

     int *_firstFrame=NULL;   // Record the first frame of each pedestrian
     int *_lastFrame=NULL;    // Record the last frame of each pedestrian
     double **_xCor=NULL;
     double **_yCor=NULL;

};

#endif /* PEDDATA_H_ */
