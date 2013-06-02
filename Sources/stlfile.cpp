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

#include <QApplication>
#include <QErrorMessage>
#include <cmath>
#include <cctype>
#include <string>
#include <algorithm>
#include <vector>

#include "stlfile.h"

#define HEADER_SIZE 84
#define JUNK_SIZE 80
#define SIZE_OF_FACET 50
#define ASCII_LINES_PER_FACET 7

static bool compareVectors(Vector i, Vector j);

static bool equalVectors(Vector i, Vector j);

StlFile::StlFile() : stats()
{
}

StlFile::~StlFile()
{
    close();
}

void StlFile::open(const ::std::string& fileName)
{
    initialize(fileName);
    computeStats();
}

void StlFile::write(const ::std::string& fileName)
{
    if(fileIn.is_open())
    {
        if(stats.type == ASCII)
        {
            writeAscii(fileName);
        }
        else
        {
            writeBinary(fileName);
        }
    }
}

void StlFile::close()
{
    fileIn.close();
}

void StlFile::setFormat(const int format)
{
    if(format == ASCII)
    {
        stats.type = ASCII;
    }
    else if(format == BINARY)
    {
        stats.type = BINARY;
    }
}

void StlFile::initialize(const ::std::string& fileName)
{
    stats.numFacets = 0;
    stats.numPoints = 0;
    stats.surface = -1.0;
    stats.volume = -1.0;
    // Open the file
    fileIn.open(fileName.c_str(), ::std::ios::in|::std::ios::binary);
    if(fileIn.is_open())
    {
        int numFacets;
        // Find length of file
        std::streampos fileSize = fileIn.tellg();
        fileIn.seekg(0, ::std::ios::end);
        fileSize = fileIn.tellg() - fileSize;
        // Check for binary or ASCII file
        fileIn.seekg(0, ::std::ios::beg);
        stats.type = BINARY;
        int c;
        while((c = fileIn.get()) != EOF && c <= 127);
        if(c == EOF)
        {
            stats.type = ASCII;
        }
        // Get the header and the number of facets in the .STL file 
        // If the .STL file is binary, then do the following 
        if(stats.type == BINARY)
        {
            fileIn.clear();
            fileIn.seekg(0, ::std::ios::beg);
            // Test if the STL file has the right size
            if(((int)fileSize - HEADER_SIZE) % SIZE_OF_FACET != 0)
            {
                ::std::cerr << "The file " << fileIn << " has a wrong size."
                            << ::std::endl;
                throw wrong_header_size();
            }
            numFacets = ((int)fileSize - HEADER_SIZE) / SIZE_OF_FACET;
            // Read the header
            char buffer[JUNK_SIZE];
            fileIn.read(buffer, JUNK_SIZE);
            stats.header = ::std::string(buffer);
            // Read the int following the header.
            // This should contain the number of facets
            int headerNumFacets = readIntFromBytes(fileIn);
            if(numFacets != headerNumFacets)
            {
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
        else
        {   // Otherwise, if the .STL file is ASCII, then do the following
            fileIn.close();
            fileIn.open(fileName.c_str(), ::std::ios::in);
            if(fileIn.is_open())
            {
                // Get the header
                char buffer[JUNK_SIZE];
                fileIn.read(buffer, JUNK_SIZE);
                stats.header = ::std::string(buffer);
                // Find the number of facets
                int numLines = 0;
                ::std::string line;
                while(!getline(fileIn, line).eof())
                {
                    if(line.size() > 4)
                    {  // don't count short lines
                        numLines++;
                    }
                }
                fileIn.clear();
                fileIn.seekg(0, ::std::ios::beg);
                numFacets = numLines / ASCII_LINES_PER_FACET;
            }
            else
            {
                ::std::cerr << "The file " << fileName << " could not be opened." << ::std::endl;
                throw error_opening_file();
            }
        }
        stats.numFacets += numFacets;
    }
    else
    {
        ::std::cerr << "The file " << fileName << " could not be opened." << ::std::endl;
        throw error_opening_file();
    }
}

void StlFile::reset()
{
    if(stats.type == BINARY)
    {
        fileIn.seekg(HEADER_SIZE, ::std::ios::beg);
    }
    else
    {
        fileIn.seekg(0, ::std::ios::beg);
        ::std::string line;
        // Skip the first line of the file
        getline(fileIn, line);
    }
}

StlFile::Facet StlFile::getNextFacet()
{
    Facet facet;
    if(stats.type == BINARY)
    {  // Read a single facet from a binary .STL file
        facet.normal.x = readFloatFromBytes(fileIn);
        facet.normal.y = readFloatFromBytes(fileIn);
        facet.normal.z = readFloatFromBytes(fileIn);
        facet.vector[0].x = readFloatFromBytes(fileIn);
        facet.vector[0].y = readFloatFromBytes(fileIn);
        facet.vector[0].z = readFloatFromBytes(fileIn);
        facet.vector[1].x = readFloatFromBytes(fileIn);
        facet.vector[1].y = readFloatFromBytes(fileIn);
        facet.vector[1].z = readFloatFromBytes(fileIn);
        facet.vector[2].x = readFloatFromBytes(fileIn);
        facet.vector[2].y = readFloatFromBytes(fileIn);
        facet.vector[2].z = readFloatFromBytes(fileIn);
        facet.extra[0] = fileIn.get();
        facet.extra[1] = fileIn.get();
    }
    else
    {  // Read a single facet from an ASCII .STL file
        ::std::string junk;
        fileIn >> junk >> junk;
        fileIn >> facet.normal.x >> facet.normal.y >> facet.normal.z;
        fileIn >> junk >> junk >> junk;
        fileIn >> facet.vector[0].x >> facet.vector[0].y >> facet.vector[0].z;
        fileIn >> junk;
        fileIn >> facet.vector[1].x >> facet.vector[1].y >> facet.vector[1].z;
        fileIn >> junk;
        fileIn >> facet.vector[2].x >> facet.vector[2].y >> facet.vector[2].z;
        fileIn >> junk >> junk;
    }

    return facet;
}

void StlFile::computeStats()
{
    float surface = 0.0;
    float volume = 0.0;
    Vector p0;
    Vector p;
    ::std::vector<Vector> vectors;
    for(int i = 0; i < stats.numFacets; i++)
    {
        Facet facet;
        facet = getNextFacet();
        // While we are going through all of the facets, let's find the
        // maximum and minimum values for x, y, and z
        // Initialize the max and min values the first time through
        if(i == 0)
        {
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

            // Choose a point, any point as the reference
            p0.x = facet.vector[0].x;
            p0.y = facet.vector[0].y;
            p0.z = facet.vector[0].z;
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

        float area = getArea(facet);
        surface += area;

        p.x = facet.vector[0].x - p0.x;
        p.y = facet.vector[0].y - p0.y;
        p.z = facet.vector[0].z - p0.z;
        // Do dot product to get distance from point to plane
        Normal n = facet.normal;
        float height = (n.x * p.x) + (n.y * p.y) + (n.z * p.z);
        volume += (area * height) / 3.0;

        for(int j = 0; j < 3; j++)
        {
            vectors.push_back(facet.vector[j]);
        }
    }
    stats.size.x = stats.max.x - stats.min.x;
    stats.size.y = stats.max.y - stats.min.y;
    stats.size.z = stats.max.z - stats.min.z;
    stats.boundingDiameter =  sqrt(stats.size.x * stats.size.x +
                                   stats.size.y * stats.size.y +
                                   stats.size.z * stats.size.z);

    ::std::sort(vectors.begin(), vectors.end(), compareVectors);
    ::std::unique(vectors.begin(), vectors.end(), equalVectors);
    stats.numPoints = vectors.size();
    stats.surface = surface > 0 ? surface : -surface;
    stats.volume = volume > 0 ? volume : -volume;
}

int StlFile::readIntFromBytes(::std::ifstream& file)
{
    int value;
    value  =  file.get() & 0xFF;
    value |= (file.get() & 0xFF) << 0x08;
    value |= (file.get() & 0xFF) << 0x10;
    value |= (file.get() & 0xFF) << 0x18;
    return(value);
}

float StlFile::readFloatFromBytes(::std::ifstream& file)
{
    union
    {
        int intValue;
        float floatValue;
    } value;
    value.intValue  =  file.get() & 0xFF;
    value.intValue |= (file.get() & 0xFF) << 0x08;
    value.intValue |= (file.get() & 0xFF) << 0x10;
    value.intValue |= (file.get() & 0xFF) << 0x18;
    return(value.floatValue);
}

void StlFile::writeBytesFromInt(::std::ofstream& file, int valueIn)
{
    union
    {
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

void StlFile::writeBytesFromFloat(::std::ofstream& file, float valueIn)
{
    union
    {
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

void StlFile::writeBinary(const ::std::string& fileName)
{
    // Open the file
    ::std::ofstream fileOut(fileName.c_str(), ::std::ios::out|::std::ios::binary);
    if(fileOut.is_open())
    {
        for(int i = 0; i < JUNK_SIZE; i++)
        {
            fileOut.put(0);
        }
        writeBytesFromInt(fileOut, stats.numFacets);
        reset();
        for(int i = 0; i < stats.numFacets; i++)
        {
            Facet facet = getNextFacet();
            writeBytesFromFloat(fileOut, facet.normal.x);
            writeBytesFromFloat(fileOut, facet.normal.y);
            writeBytesFromFloat(fileOut, facet.normal.z);
            writeBytesFromFloat(fileOut, facet.vector[0].x);
            writeBytesFromFloat(fileOut, facet.vector[0].y);
            writeBytesFromFloat(fileOut, facet.vector[0].z);
            writeBytesFromFloat(fileOut, facet.vector[1].x);
            writeBytesFromFloat(fileOut, facet.vector[1].y);
            writeBytesFromFloat(fileOut, facet.vector[1].z);
            writeBytesFromFloat(fileOut, facet.vector[2].x);
            writeBytesFromFloat(fileOut, facet.vector[2].y);
            writeBytesFromFloat(fileOut, facet.vector[2].z);
            fileOut << facet.extra[0];
            fileOut << facet.extra[1];
        }
        fileOut.close();
    }
    else
    {
        ::std::cerr << "The file " << fileName << " could not be found." << ::std::endl;
        throw error_opening_file();
    }
}

void StlFile::writeAscii(const ::std::string& fileName)
{
    // Open the file
    ::std::ofstream fileOut(fileName.c_str(), ::std::ios::out);
    fileOut.setf(::std::ios::scientific);
    fileOut.precision(8);
    if(fileOut.is_open())
    {
        reset();
        fileOut << "solid" << ::std::endl;
        for(int i = 0; i < stats.numFacets; i++)
        {
            Facet facet = getNextFacet();
            fileOut << "  facet normal " << facet.normal.x << " "
                 << facet.normal.y << " " << facet.normal.z << ::std::endl;
            fileOut << "    outer loop " << ::std::endl;
            fileOut << "      vertex " << facet.vector[0].x << " "
                 << facet.vector[0].y << " " << facet.vector[0].z
                 << ::std::endl;
            fileOut << "      vertex " << facet.vector[1].x << " "
                 << facet.vector[1].y << " " << facet.vector[1].z
                 << ::std::endl;
            fileOut << "      vertex " << facet.vector[2].x << " "
                 << facet.vector[2].y << " " << facet.vector[2].z
                 << ::std::endl;
            fileOut << "    endloop" << ::std::endl;
            fileOut << "  endfacet" << ::std::endl;
        }
        fileOut << "endsolid" << ::std::endl;
        fileOut.close();
    }
    else
    {
        ::std::cerr << "The file " << fileName << " could not be found." << ::std::endl;
        throw error_opening_file();
    }
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

float StlFile::getArea(Facet &facet)
{
    float cross[3][3];
    float sum[3];
    float n[3];
    for(int i = 0; i < 3; i++)
    {
        cross[i][0] = ((facet.vector[i].y * facet.vector[(i + 1) % 3].z) -
        (facet.vector[i].z * facet.vector[(i + 1) % 3].y));
        cross[i][1] = ((facet.vector[i].z * facet.vector[(i + 1) % 3].x) -
        (facet.vector[i].x * facet.vector[(i + 1) % 3].z));
        cross[i][2] = ((facet.vector[i].x * facet.vector[(i + 1) % 3].y) -
        (facet.vector[i].y * facet.vector[(i + 1) % 3].x));
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

void StlFile::calculateNormal(float normal[], Facet &facet)
{
    float v1[3];
    float v2[3];
    v1[0] = facet.vector[1].x - facet.vector[0].x;
    v1[1] = facet.vector[1].y - facet.vector[0].y;
    v1[2] = facet.vector[1].z - facet.vector[0].z;
    v2[0] = facet.vector[2].x - facet.vector[0].x;
    v2[1] = facet.vector[2].y - facet.vector[0].y;
    v2[2] = facet.vector[2].z - facet.vector[0].z;
    normal[0] = (float)((double)v1[1] * (double)v2[2])
                - ((double)v1[2] * (double)v2[1]);
    normal[1] = (float)((double)v1[2] * (double)v2[0])
                - ((double)v1[0] * (double)v2[2]);
    normal[2] = (float)((double)v1[0] * (double)v2[1])
                - ((double)v1[1] * (double)v2[0]);
}

void StlFile::normalizeVector(float v[])
{
    double length = sqrt((double)v[0] * (double)v[0]
                         + (double)v[1] * (double)v[1]
                            + (double)v[2] * (double)v[2]);
    float minNormalLength = 0.000000000001;
    if(length < minNormalLength)
    {
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
