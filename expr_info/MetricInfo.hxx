////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#ifndef METRICINFO_H
#define METRICINFO_H

class MetricInfo
{
public:
    MetricInfo();

    MetricInfo(int i, float p, float es, char *_functionName, char *_fileName, int fl, int s, int e);

    ~MetricInfo();

    int index;
    float percent;
    float exclusive_seconds;
    char *functionName;
    char *fileName;
    int function_line_number;
    int start;
    int end;
};
#endif // METRICINFO_H
