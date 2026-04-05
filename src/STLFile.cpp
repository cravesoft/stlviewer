// Copyright (C) 2009-2015 Olivier Crave
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
#include <QDebug>
#include <cmath>
#include <cctype>
#include <string>
#include <algorithm>
#include <vector>

#include "STLFile.hpp"

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
    this->close();
}

void StlFile::open(const ::std::string& fileName)
{
    this->initialize(fileName);
    this->computeStats();
}

void StlFile::write(const ::std::string& fileName)
{
    if (this->fileIn.is_open())
    {
        if (this->stats.type == ASCII)
            this->writeAscii(fileName);
        else
            this->writeBinary(fileName);
    }
}

void StlFile::close()
{
    this->fileIn.close();
}

void StlFile::setFormat(const int format)
{
    if (format == ASCII)
        this->stats.type = ASCII;
    else if (format == BINARY)
        this->stats.type = BINARY;
}

void StlFile::initialize(const ::std::string& fileName)
{
    this->stats.numFacets = 0;
    this->stats.numPoints = 0;
    this->stats.surface = -1.0f;
    this->stats.volume = -1.0f;
    fileIn.open(fileName.c_str(), ::std::ios::in | ::std::ios::binary);
    if (fileIn.is_open())
    {
        int numFacets;
        std::streampos fileSize = fileIn.tellg();
        fileIn.seekg(0, ::std::ios::end);
        fileSize = fileIn.tellg() - fileSize;
        fileIn.seekg(0, ::std::ios::beg);
        this->stats.type = BINARY;
        int c;
        while ((c = fileIn.get()) != EOF && c <= 127);
        if (c == EOF)
            this->stats.type = ASCII;
        this->inputType = this->stats.type;
        if (this->stats.type == BINARY)
        {
            fileIn.clear();
            fileIn.seekg(0, ::std::ios::beg);
            if (((int)fileSize - HEADER_SIZE) % SIZE_OF_FACET != 0)
            {
                qWarning() << "The file" << fileName.c_str() << "has a wrong size.";
                throw wrong_header_size();
            }
            numFacets = ((int)fileSize - HEADER_SIZE) / SIZE_OF_FACET;
            char buffer[JUNK_SIZE];
            fileIn.read(buffer, JUNK_SIZE);
            this->stats.header = ::std::string(buffer);
            int headerNumFacets = readIntFromBytes(fileIn);
            if (numFacets != headerNumFacets)
            {
                qWarning() << "File size doesn't match number of facets in the header.";
                QErrorMessage errMessage;
                errMessage.showMessage("File size doesn't match number of facets in the header.");
                QApplication::restoreOverrideCursor();
                errMessage.exec();
                QApplication::setOverrideCursor(Qt::WaitCursor);
            }
        }
        else
        {
            fileIn.close();
            fileIn.open(fileName.c_str(), ::std::ios::in);
            if (fileIn.is_open())
            {
                char buffer[JUNK_SIZE];
                fileIn.read(buffer, JUNK_SIZE);
                this->stats.header = ::std::string(buffer);
                int numLines = 0;
                ::std::string line;
                while (!getline(fileIn, line).eof())
                {
                    if (line.size() > 4)
                        numLines++;
                }
                fileIn.clear();
                fileIn.seekg(0, ::std::ios::beg);
                numFacets = numLines / ASCII_LINES_PER_FACET;
            }
            else
            {
                qWarning() << "The file" << fileName.c_str() << "could not be opened.";
                throw error_opening_file();
            }
        }
        this->stats.numFacets += numFacets;
    }
    else
    {
        qWarning() << "The file" << fileName.c_str() << "could not be opened.";
        throw error_opening_file();
    }
}

void StlFile::reset()
{
    if (this->inputType == BINARY)
        this->fileIn.seekg(HEADER_SIZE, ::std::ios::beg);
    else
    {
        this->fileIn.seekg(0, ::std::ios::beg);
        ::std::string line;
        getline(this->fileIn, line);
    }
}

StlFile::Facet StlFile::getNextFacet()
{
    Facet facet;
    if (this->inputType == BINARY)
    {
        facet.normal.x = readFloatFromBytes(fileIn);
        facet.normal.y = readFloatFromBytes(fileIn);
        facet.normal.z = readFloatFromBytes(fileIn);
        for (int i = 0; i < 3; i++)
        {
            facet.vector[i].x = readFloatFromBytes(fileIn);
            facet.vector[i].y = readFloatFromBytes(fileIn);
            facet.vector[i].z = readFloatFromBytes(fileIn);
        }
        facet.extra[0] = fileIn.get();
        facet.extra[1] = fileIn.get();
    }
    else
    {
        ::std::string keyword;
        float x, y, z;
        fileIn >> keyword >> keyword >> x >> y >> z;
        facet.normal = Normal{x, y, z};
        fileIn >> keyword >> keyword;
        for (int i = 0; i < 3; i++)
        {
            fileIn >> keyword >> x >> y >> z;
            facet.vector[i] = Vector{x, y, z};
        }
        fileIn >> keyword;
        fileIn >> keyword;
    }
    return facet;
}

void StlFile::computeStats()
{
    this->reset();
    float surface = 0.0f;
    float volume  = 0.0f;
    Vector p0;
    ::std::vector<Vector> vectors;

    for (int i = 0; i < this->stats.numFacets; i++)
    {
        Facet facet = getNextFacet();

        if (i == 0)
        {
            this->stats.max = facet.vector[0];
            this->stats.min = facet.vector[0];

            float xDiff = std::abs(facet.vector[0].x - facet.vector[1].x);
            float yDiff = std::abs(facet.vector[0].y - facet.vector[1].y);
            float zDiff = std::abs(facet.vector[0].z - facet.vector[1].z);
            float maxDiff = std::max({xDiff, yDiff, zDiff});
            this->stats.shortestEdge = maxDiff;
            p0 = facet.vector[0];
        }

        for (int j = 0; j < 3; j++)
        {
            this->stats.max.x = std::max(this->stats.max.x, facet.vector[j].x);
            this->stats.max.y = std::max(this->stats.max.y, facet.vector[j].y);
            this->stats.max.z = std::max(this->stats.max.z, facet.vector[j].z);
            this->stats.min.x = std::min(this->stats.min.x, facet.vector[j].x);
            this->stats.min.y = std::min(this->stats.min.y, facet.vector[j].y);
            this->stats.min.z = std::min(this->stats.min.z, facet.vector[j].z);
        }

        float area = this->getArea(facet);
        surface += area;

        Vector p;
        p.x = facet.vector[0].x - p0.x;
        p.y = facet.vector[0].y - p0.y;
        p.z = facet.vector[0].z - p0.z;
        float height = facet.normal.x * p.x + facet.normal.y * p.y + facet.normal.z * p.z;
        volume += (area * height) / 3.0f;

        for (int j = 0; j < 3; j++)
            vectors.push_back(facet.vector[j]);
    }

    this->stats.size.x = this->stats.max.x - this->stats.min.x;
    this->stats.size.y = this->stats.max.y - this->stats.min.y;
    this->stats.size.z = this->stats.max.z - this->stats.min.z;
    this->stats.boundingDiameter = std::sqrt(
        this->stats.size.x * this->stats.size.x +
        this->stats.size.y * this->stats.size.y +
        this->stats.size.z * this->stats.size.z);

    ::std::sort(vectors.begin(), vectors.end(), compareVectors);
    auto last = ::std::unique(vectors.begin(), vectors.end(), equalVectors);
    this->stats.numPoints = static_cast<int>(last - vectors.begin());
    this->stats.surface = surface > 0 ? surface : -surface;
    this->stats.volume  = volume  > 0 ? volume  : -volume;
}

int StlFile::readIntFromBytes(::std::ifstream& file)
{
    int value;
    value  =  file.get() & 0xFF;
    value |= (file.get() & 0xFF) << 0x08;
    value |= (file.get() & 0xFF) << 0x10;
    value |= (file.get() & 0xFF) << 0x18;
    return value;
}

float StlFile::readFloatFromBytes(::std::ifstream& file)
{
    union { int intValue; float floatValue; } value;
    value.intValue  =  file.get() & 0xFF;
    value.intValue |= (file.get() & 0xFF) << 0x08;
    value.intValue |= (file.get() & 0xFF) << 0x10;
    value.intValue |= (file.get() & 0xFF) << 0x18;
    return value.floatValue;
}

void StlFile::writeBytesFromInt(::std::ofstream& file, int valueIn)
{
    union { int intValue; char charValue[4]; } value;
    value.intValue = valueIn;
    int newValue  = value.charValue[0] & 0xFF;
    newValue |= (value.charValue[1] & 0xFF) << 0x08;
    newValue |= (value.charValue[2] & 0xFF) << 0x10;
    newValue |= (value.charValue[3] & 0xFF) << 0x18;
    file.write(reinterpret_cast<char*>(&newValue), sizeof(newValue));
}

void StlFile::writeBytesFromFloat(::std::ofstream& file, float valueIn)
{
    union { float floatValue; char charValue[4]; } value;
    value.floatValue = valueIn;
    int newValue  = value.charValue[0] & 0xFF;
    newValue |= (value.charValue[1] & 0xFF) << 0x08;
    newValue |= (value.charValue[2] & 0xFF) << 0x10;
    newValue |= (value.charValue[3] & 0xFF) << 0x18;
    file.write(reinterpret_cast<char*>(&newValue), sizeof(newValue));
}

void StlFile::writeBinary(const ::std::string& fileName)
{
    ::std::ofstream fileOut(fileName.c_str(), ::std::ios::out | ::std::ios::binary);
    if (fileOut.is_open())
    {
        for (int i = 0; i < JUNK_SIZE; i++)
            fileOut.put(0);
        writeBytesFromInt(fileOut, this->stats.numFacets);
        this->reset();
        for (int i = 0; i < this->stats.numFacets; i++)
        {
            Facet facet = getNextFacet();
            writeBytesFromFloat(fileOut, facet.normal.x);
            writeBytesFromFloat(fileOut, facet.normal.y);
            writeBytesFromFloat(fileOut, facet.normal.z);
            for (int j = 0; j < 3; j++)
            {
                writeBytesFromFloat(fileOut, facet.vector[j].x);
                writeBytesFromFloat(fileOut, facet.vector[j].y);
                writeBytesFromFloat(fileOut, facet.vector[j].z);
            }
            fileOut << facet.extra[0];
            fileOut << facet.extra[1];
        }
        fileOut.close();
    }
    else
    {
        qWarning() << "The file" << fileName.c_str() << "could not be opened for writing.";
        throw error_opening_file();
    }
}

void StlFile::writeAscii(const ::std::string& fileName)
{
    ::std::ofstream fileOut(fileName.c_str(), ::std::ios::out);
    fileOut.setf(::std::ios::scientific);
    fileOut.precision(8);
    if (fileOut.is_open())
    {
        this->reset();
        fileOut << "solid" << ::std::endl;
        for (int i = 0; i < this->stats.numFacets; i++)
        {
            Facet facet = getNextFacet();
            fileOut << "  facet normal " << facet.normal.x << " "
                    << facet.normal.y << " " << facet.normal.z << ::std::endl;
            fileOut << "    outer loop" << ::std::endl;
            for (int j = 0; j < 3; j++)
                fileOut << "      vertex " << facet.vector[j].x << " "
                        << facet.vector[j].y << " " << facet.vector[j].z << ::std::endl;
            fileOut << "    endloop" << ::std::endl;
            fileOut << "  endfacet" << ::std::endl;
        }
        fileOut << "endsolid" << ::std::endl;
        fileOut.close();
    }
    else
    {
        qWarning() << "The file" << fileName.c_str() << "could not be opened for writing.";
        throw error_opening_file();
    }
}

static bool compareVectors(Vector i, Vector j)
{
    if (i.x != j.x) return i.x < j.x;
    if (i.y != j.y) return i.y < j.y;
    return i.z < j.z;
}

static bool equalVectors(Vector i, Vector j)
{
    return i.x == j.x && i.y == j.y && i.z == j.z;
}

float StlFile::getArea(Facet& facet)
{
    float normal[3];
    calculateNormal(normal, facet);
    normalizeVector(normal);

    float cross[3][3];
    float sum[3];
    for (int i = 0; i < 3; i++)
    {
        cross[i][0] = facet.vector[i].y * facet.vector[(i+1)%3].z
                    - facet.vector[i].z * facet.vector[(i+1)%3].y;
        cross[i][1] = facet.vector[i].z * facet.vector[(i+1)%3].x
                    - facet.vector[i].x * facet.vector[(i+1)%3].z;
        cross[i][2] = facet.vector[i].x * facet.vector[(i+1)%3].y
                    - facet.vector[i].y * facet.vector[(i+1)%3].x;
    }
    sum[0] = cross[0][0] + cross[1][0] + cross[2][0];
    sum[1] = cross[0][1] + cross[1][1] + cross[2][1];
    sum[2] = cross[0][2] + cross[1][2] + cross[2][2];

    return 0.5f * std::abs(normal[0]*sum[0] + normal[1]*sum[1] + normal[2]*sum[2]);
}

void StlFile::calculateNormal(float normal[], Facet& facet)
{
    float v1[3] = {
        facet.vector[1].x - facet.vector[0].x,
        facet.vector[1].y - facet.vector[0].y,
        facet.vector[1].z - facet.vector[0].z
    };
    float v2[3] = {
        facet.vector[2].x - facet.vector[0].x,
        facet.vector[2].y - facet.vector[0].y,
        facet.vector[2].z - facet.vector[0].z
    };
    normal[0] = v1[1]*v2[2] - v1[2]*v2[1];
    normal[1] = v1[2]*v2[0] - v1[0]*v2[2];
    normal[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void StlFile::normalizeVector(float v[])
{
    float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0f)
    {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}
