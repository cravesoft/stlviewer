// Copyright (c) 2009 Olivier Crave
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <QtGlobal>
#include <QApplication>
#include <QErrorMessage>
#include <math.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

#include "stlfile.h"

#define HEADER_SIZE 84
#define JUNK_SIZE 80
#define SIZE_OF_FACET 50
#define ASCII_LINES_PER_FACET 7

StlFile::StlFile() {
  facets = 0;
}

StlFile::~StlFile() {
  close();
}

void StlFile::open(const ::std::string& fileName) {
  initialize(fileName);
  allocate();
  readData(0, 1);
  file.close();
}

void StlFile::write(const ::std::string& fileName) {
  if (facets != 0) {
    if (stats.type == ASCII)
      writeAscii(fileName);
    else
      writeBinary(fileName);
  }
}

void StlFile::close() {
  if (facets != 0) {
	  delete[] facets;
    facets = 0;
  }
}

void StlFile::setFormat(const int format) {
  if (format == ASCII)
    stats.type = ASCII;
  else if (format == BINARY)
    stats.type = BINARY;
}

void StlFile::initialize(const ::std::string& fileName) {
  stats.numFacets = 0;
  stats.numPoints = 0;
  stats.surface = -1.0;
  stats.volume = -1.0;
  facets = 0;
  // Open the file
  file.open(fileName.c_str(), ::std::ios::binary);
  if (file.is_open()) {
    int numFacets;
    // Find length of file
    file.seekg(0, ::std::ios::end);
    int fileSize = file.tellg();
    // Check for binary or ASCII file
    file.seekg(0, ::std::ios::beg);
    stats.type = BINARY;
    int c;
    while((c = file.get()) != EOF && c <= 127) 
        ;
    if(c == EOF) {
      stats.type = ASCII;
    }
    file.seekg(0, ::std::ios::beg);
    // Get the header and the number of facets in the .STL file 
    // If the .STL file is binary, then do the following 
    if (stats.type == BINARY) {
      // Test if the STL file has the right size
      if ((fileSize - HEADER_SIZE) % SIZE_OF_FACET != 0) {
        ::std::cerr << "The file " << file << " has a wrong size."
                    << ::std::endl;
        throw wrong_header_size();
      }
      numFacets = (fileSize - HEADER_SIZE) / SIZE_OF_FACET;
      // Read the header
      char buffer[JUNK_SIZE];
      file.read(buffer, JUNK_SIZE);
      stats.header = buffer;
      // Read the int following the header.
      // This should contain the number of facets
      int headerNumFacets = readIntFromBytes(file);
      if (numFacets != headerNumFacets) {
        ::std::cerr << "Warning: File size doesn't match number of "
                    << "facets in the header." << ::std::endl;
        QErrorMessage errMessage;
        errMessage.showMessage("File size doesn't match number of facets "
                               "in the header.");
        QApplication::restoreOverrideCursor();
        errMessage.exec();
        QApplication::setOverrideCursor(Qt::WaitCursor);
      }
    }
    else {  // Otherwise, if the .STL file is ASCII, then do the following
      file.seekg(0, ::std::ios::beg);
      // Get the header
      getline(file, stats.header);
      // Find the number of facets
      int numLines = 0;
      ::std::string line;
      while (!getline(file, line).eof()) {
        if (line.size() > 4) {  // don't count short lines
          numLines++;
        }
      }
      file.seekg(0, ::std::ios::beg);
      numFacets = numLines / ASCII_LINES_PER_FACET;
    }
    stats.numFacets += numFacets;
  } else {
    ::std::cerr << "The file " << file << " could not be found." << ::std::endl;
    throw error_opening_file();
  }
}

void StlFile::allocate() {
  // Allocate memory for the entire .STL file
  facets = new Facet[stats.numFacets];
  if (facets == 0) {
    ::std::cerr << "Problem allocating memory" << ::std::endl;
    throw ::std::bad_alloc();
  }
}

void StlFile::readData(int firstFacet, int first) {
  if (stats.type == BINARY) {
    file.seekg(HEADER_SIZE, ::std::ios::beg);
  } else {
    file.seekg(0, ::std::ios::beg);
    ::std::string line;
    // Skip the first line of the file
    getline(file, line);
  }
  Facet facet;
  for (int i = firstFacet; i < stats.numFacets; i++) {
    if (stats.type == BINARY) {  // Read a single facet from a binary .STL file
	    facet.normal.x = readFloatFromBytes(file);
	    facet.normal.y = readFloatFromBytes(file);
	    facet.normal.z = readFloatFromBytes(file);
	    facet.vector[0].x = readFloatFromBytes(file);
	    facet.vector[0].y = readFloatFromBytes(file);
	    facet.vector[0].z = readFloatFromBytes(file);
	    facet.vector[1].x = readFloatFromBytes(file);
	    facet.vector[1].y = readFloatFromBytes(file);
	    facet.vector[1].z = readFloatFromBytes(file);
	    facet.vector[2].x = readFloatFromBytes(file);
	    facet.vector[2].y = readFloatFromBytes(file);
	    facet.vector[2].z = readFloatFromBytes(file);
	    facet.extra[0] = file.get();
	    facet.extra[1] = file.get();
    } else {  // Read a single facet from an ASCII .STL file
      ::std::string junk;
      file >> junk >> junk;
      file >> facet.normal.x >> facet.normal.y >> facet.normal.z;
      file >> junk >> junk >> junk;
      file >> facet.vector[0].x >> facet.vector[0].y >> facet.vector[0].z;
      file >> junk;
      file >> facet.vector[1].x >> facet.vector[1].y >> facet.vector[1].z;
      file >> junk;
      file >> facet.vector[2].x >> facet.vector[2].y >> facet.vector[2].z;
      file >> junk >> junk;
    }
    // Write the facet into memory.
    facets[i] = facet;
    // While we are going through all of the facets, let's find the
    // maximum and minimum values for x, y, and z
    // Initialize the max and min values the first time through
    if (first) {
	    stats.max.x = facet.vector[0].x;
	    stats.min.x = facet.vector[0].x;
	    stats.max.y = facet.vector[0].y;
	    stats.min.y = facet.vector[0].y;
	    stats.max.z = facet.vector[0].z;
	    stats.min.z = facet.vector[0].z;
  	  
	    float xDiff = qAbs(facet.vector[0].x - facet.vector[1].x);
	    float yDiff = qAbs(facet.vector[0].y - facet.vector[1].y);
	    float zDiff = qAbs(facet.vector[0].z - facet.vector[1].z);
	    float maxDiff = qMax(xDiff, yDiff);
	    maxDiff = qMax(zDiff, maxDiff);
	    stats.shortestEdge = maxDiff;

      first = 0;
    }
    // Now find the max and min values
    stats.max.x = qMax(stats.max.x, facet.vector[0].x);
    stats.min.x = qMin(stats.min.x, facet.vector[0].x);
    stats.max.y = qMax(stats.max.y, facet.vector[0].y);
    stats.min.y = qMin(stats.min.y, facet.vector[0].y);
    stats.max.z = qMax(stats.max.z, facet.vector[0].z);
    stats.min.z = qMin(stats.min.z, facet.vector[0].z);

    stats.max.x = qMax(stats.max.x, facet.vector[1].x);
    stats.min.x = qMin(stats.min.x, facet.vector[1].x);
    stats.max.y = qMax(stats.max.y, facet.vector[1].y);
    stats.min.y = qMin(stats.min.y, facet.vector[1].y);
    stats.max.z = qMax(stats.max.z, facet.vector[1].z);
    stats.min.z = qMin(stats.min.z, facet.vector[1].z);

    stats.max.x = qMax(stats.max.x, facet.vector[2].x);
    stats.min.x = qMin(stats.min.x, facet.vector[2].x);
    stats.max.y = qMax(stats.max.y, facet.vector[2].y);
    stats.min.y = qMin(stats.min.y, facet.vector[2].y);
    stats.max.z = qMax(stats.max.z, facet.vector[2].z);
    stats.min.z = qMin(stats.min.z, facet.vector[2].z);
  }
  stats.size.x = stats.max.x - stats.min.x;
  stats.size.y = stats.max.y - stats.min.y;
  stats.size.z = stats.max.z - stats.min.z;
  stats.boundingDiameter =  sqrt(stats.size.x * stats.size.x +
                                   stats.size.y * stats.size.y +
                                   stats.size.z * stats.size.z);
  stats.numPoints = getNumPoints();
  stats.surface = getSurface();
  stats.volume = getVolume();
}

int StlFile::readIntFromBytes(::std::ifstream& file) {
  int value;
  value  =  file.get() & 0xFF;
  value |= (file.get() & 0xFF) << 0x08;
  value |= (file.get() & 0xFF) << 0x10;
  value |= (file.get() & 0xFF) << 0x18;
  return(value);
}

float StlFile::readFloatFromBytes(::std::ifstream& file) {
  union {
    int intValue;
    float floatValue;
  } value;
  value.intValue  =  file.get() & 0xFF;
  value.intValue |= (file.get() & 0xFF) << 0x08;
  value.intValue |= (file.get() & 0xFF) << 0x10;
  value.intValue |= (file.get() & 0xFF) << 0x18;
  return(value.floatValue);
}

void StlFile::writeBytesFromInt(::std::ofstream& file, int valueIn) {
  union {
    int intValue;
    char charValue[4];
  } value;
  value.intValue = valueIn;
  int newValue  = value.charValue[0] & 0xFF;
  newValue |= (value.charValue[1] & 0xFF) << 0x08;
  newValue |= (value.charValue[2] & 0xFF) << 0x10;
  newValue |= (value.charValue[3] & 0xFF) << 0x18;
  file.write(reinterpret_cast<char *>(&newValue), sizeof(newValue));
}

void StlFile::writeBytesFromFloat(::std::ofstream& file, float valueIn) {
  union {
    float floatValue;
    char charValue[4];
  } value;
  value.floatValue = valueIn;
  int newValue  = value.charValue[0] & 0xFF;
  newValue |= (value.charValue[1] & 0xFF) << 0x08;
  newValue |= (value.charValue[2] & 0xFF) << 0x10;
  newValue |= (value.charValue[3] & 0xFF) << 0x18;
  file.write(reinterpret_cast<char *>(&newValue), sizeof(newValue));
}

void StlFile::writeBinary(const ::std::string& fileName) {
  // Open the file
  ::std::ofstream file(fileName.c_str(), ::std::ios::out|::std::ios::binary);
  if (file.is_open()) {
    for (int i = 0; i < JUNK_SIZE; i++) file.put(0);
    writeBytesFromInt(file, stats.numFacets);
    for (int i = 0; i < stats.numFacets; i++) {
      writeBytesFromFloat(file, facets[i].normal.x);
      writeBytesFromFloat(file, facets[i].normal.y);
      writeBytesFromFloat(file, facets[i].normal.z);
      writeBytesFromFloat(file, facets[i].vector[0].x);
      writeBytesFromFloat(file, facets[i].vector[0].y);
      writeBytesFromFloat(file, facets[i].vector[0].z);
      writeBytesFromFloat(file, facets[i].vector[1].x);
      writeBytesFromFloat(file, facets[i].vector[1].y);
      writeBytesFromFloat(file, facets[i].vector[1].z);
      writeBytesFromFloat(file, facets[i].vector[2].x);
      writeBytesFromFloat(file, facets[i].vector[2].y);
      writeBytesFromFloat(file, facets[i].vector[2].z);
      file << facets[i].extra[0];
      file << facets[i].extra[1];
    }
    file.close();
  } else {
    ::std::cerr << "The file " << file << " could not be found." << ::std::endl;
    throw error_opening_file();
  }
}

void StlFile::writeAscii(const ::std::string& fileName) {
  // Open the file
  ::std::ofstream file(fileName.c_str(), ::std::ios::out);
  file.setf(::std::ios::scientific);
  file.precision(8);
  if (file.is_open()) {
    file << "solid" << ::std::endl;
    for (int i = 0; i < stats.numFacets; i++) {
      file << "  facet normal " << facets[i].normal.x << " "
           << facets[i].normal.y << " " << facets[i].normal.z << ::std::endl;
      file << "    outer loop " << ::std::endl;
      file << "      Vector " << facets[i].vector[0].x << " "
           << facets[i].vector[0].y << " " << facets[i].vector[0].z
           << ::std::endl;
      file << "      Vector " << facets[i].vector[1].x << " " 
           << facets[i].vector[1].y << " " << facets[i].vector[1].z
           << ::std::endl;
      file << "      Vector " << facets[i].vector[2].x << " "
           << facets[i].vector[2].y << " " << facets[i].vector[2].z
           << ::std::endl;
      file << "    endloop" << ::std::endl;
      file << "  endfacet" << ::std::endl;
    }
    file << "endsolid" << ::std::endl;
    file.close();
  } else {
    ::std::cerr << "The file " << file << " could not be found." << ::std::endl;
    throw error_opening_file();
  }
  /*FILE *fp = fopen(fileName.c_str(), "w");
  fprintf(fp, "solid\n");
  for (int i = 0; i < stats.numFacets; i++) {
    fprintf(fp, "  facet normal % 0.8f % 0.8f % 0.8f\n",
    facets[i].normal.x, facets[i].normal.y,
    facets[i].normal.z);
    fprintf(fp, "    outer loop\n");
    fprintf(fp, "      vertex % 0.8f % 0.8f % 0.8f\n",
    facets[i].vector[0].x, facets[i].vector[0].y,
    facets[i].vector[0].z);
    fprintf(fp, "      vertex % 0.8f % 0.8f % 0.8f\n",
    facets[i].vector[1].x, facets[i].vector[1].y,
    facets[i].vector[1].z);
    fprintf(fp, "      vertex % 0.8f % 0.8f % 0.8f\n",
    facets[i].vector[2].x, facets[i].vector[2].y,
    facets[i].vector[2].z);
    fprintf(fp, "    endloop\n");
    fprintf(fp, "  endfacet\n");
  }
  fprintf(fp, "endsolid\n");
  fclose(fp);*/
}

static bool compareVectors(Vector i, Vector j)
{
  Vector diff;
  diff = i - j;
  return (diff.Magnitude() < 0);
}

static bool equalVectors(Vector i, Vector j)
{
  Vector diff;
  diff = i - j;
  return (diff.Magnitude() == 0);
}

int StlFile::getNumPoints() {
  ::std::vector<Vector> vectors;
  for (int i = 0; i < stats.numFacets; i++) {
    for (int j = 0; j < 3; j++) {
      vectors.push_back(facets[i].vector[j]);
    }
  }
  ::std::sort(vectors.begin(), vectors.end(), compareVectors);
  ::std::unique(vectors.begin(), vectors.end(), equalVectors);
  return vectors.size();
}

float StlFile::getVolume() {
  Vector p0;
  Vector p;
  float volume = 0.0;
  // Choose a point, any point as the reference
  p0.x = facets[0].vector[0].x;
  p0.y = facets[0].vector[0].y;
  p0.z = facets[0].vector[0].z;
  for (int i = 0; i < stats.numFacets; i++) {
    p.x = facets[i].vector[0].x - p0.x;
    p.y = facets[i].vector[0].y - p0.y;
    p.z = facets[i].vector[0].z - p0.z;
    // Do dot product to get distance from point to plane
    Normal n = facets[i].normal;
    float height = (n.x * p.x) + (n.y * p.y) + (n.z * p.z);
    float area = getArea(&facets[i]);
    volume += (area * height) / 3.0;
  }
	if (volume < 0.0) {
		volume = -volume;
	}
  return volume;
}

float StlFile::getSurface() {
  float surface = 0.0;
  for (int i = 0; i < stats.numFacets; i++) {
    float area = getArea(&facets[i]);
    surface += area;
  }
	if (surface < 0.0) {
		surface = -surface;
	}
  return surface;
}

float StlFile::getArea(Facet *facet) {
	float cross[3][3];
	float sum[3];
	float n[3];
	for (int i = 0; i < 3; i++) {
    cross[i][0] = ((facet->vector[i].y * facet->vector[(i + 1) % 3].z) -
    (facet->vector[i].z * facet->vector[(i + 1) % 3].y));
    cross[i][1] = ((facet->vector[i].z * facet->vector[(i + 1) % 3].x) -
    (facet->vector[i].x * facet->vector[(i + 1) % 3].z));
    cross[i][2] = ((facet->vector[i].x * facet->vector[(i + 1) % 3].y) -
    (facet->vector[i].y * facet->vector[(i + 1) % 3].x));
	}
	sum[0] = cross[0][0] + cross[1][0] + cross[2][0];
	sum[1] = cross[0][1] + cross[1][1] + cross[2][1];
	sum[2] = cross[0][2] + cross[1][2] + cross[2][2];
	// This should already be done.  But just in case, let's do it again
	calculateNormal(n, facet);
	normalizeVector(n);
	float area = 0.5 * (n[0] * sum[0] + n[1] * sum[1] + n[2] * sum[2]);
	return area;
}

void StlFile::calculateNormal(float normal[], Facet *facet) {
  float v1[3];
  float v2[3];
  v1[0] = facet->vector[1].x - facet->vector[0].x;
  v1[1] = facet->vector[1].y - facet->vector[0].y;
  v1[2] = facet->vector[1].z - facet->vector[0].z;
  v2[0] = facet->vector[2].x - facet->vector[0].x;
  v2[1] = facet->vector[2].y - facet->vector[0].y;
  v2[2] = facet->vector[2].z - facet->vector[0].z;
  normal[0] = (float)((double)v1[1] * (double)v2[2])
              - ((double)v1[2] * (double)v2[1]);
  normal[1] = (float)((double)v1[2] * (double)v2[0])
              - ((double)v1[0] * (double)v2[2]);
  normal[2] = (float)((double)v1[0] * (double)v2[1])
              - ((double)v1[1] * (double)v2[0]);
}

void StlFile::normalizeVector(float v[]) {
  double length = sqrt((double)v[0] * (double)v[0]
                       + (double)v[1] * (double)v[1]
                          + (double)v[2] * (double)v[2]);
  float minNormalLength = 0.000000000001;
  if (length < minNormalLength) {
    v[0] = 1.0;
    v[1] = 0.0;
    v[2] = 0.0;
    return;
  }  
  double factor = 1.0 / length;
  v[0] *= factor;
  v[1] *= factor;
  v[2] *= factor;
}
