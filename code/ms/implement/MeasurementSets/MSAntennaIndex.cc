//# MSAntennaIndex.cc: implementation of MSAntennaIndex.h
//# Copyright (C) 2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <trial/MeasurementSets/MSAntennaIndex.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayUtil.h>

//-------------------------------------------------------------------------

MSAntennaIndex::MSAntennaIndex(const MSAntenna& antenna)
  : msAntennaCols_p(antenna)
{ 
// Construct from an MS ANTENNA subtable
// Input:
//    antenna           const MSAntenna&           Input MSAntenna object
// Output to private data:
//    msAntennaCols_p   ROMSAntennaColumns         MSAntenna columns accessor
//    antennaIds_p      Vector<Int>                Antenna id's
//    nrows_p           Int                        Number of rows
//
  // Generate an array of antenna id's, used in later queries
  nrows_p = msAntennaCols_p.nrow();
  antennaIds_p.resize(nrows_p);
  indgen(antennaIds_p);
};

//-------------------------------------------------------------------------

Vector<Int> MSAntennaIndex::matchAntennaName(const String& name)
{
// Match a antenna name to a set of antenna id's
// Input:
//    name               const String&            Antenna name to match
// Output:
//    matchAntennaName   Vector<Int>              Matching antenna id's
//
  LogicalArray maskArray = (msAntennaCols_p.name().getColumn()==name &&
			    !msAntennaCols_p.flagRow().getColumn());
  MaskedArray<Int> maskAntennaId(antennaIds_p, maskArray);
  return maskAntennaId.getCompressedArray();
}; 

//-------------------------------------------------------------------------

Vector<Int> MSAntennaIndex::matchAntennaName(const Vector<String>& names)
{
// Match a set of antenna names to a set of antenna id's
// Input:
//    names              const Vector<String>&    Antenna names to match
// Output:
//    matchAntennaNames  Vector<Int>              Matching antenna id's
//
  Vector<Int> matchedAntennaIds;
  // Match each antenna name individually
  for (uInt fld=0; fld<names.nelements(); fld++) {
    // Add to list of antenna id's
    Vector<Int> currentMatch = matchAntennaName(names(fld));
    if (currentMatch.nelements() > 0) {
      Vector<Int> temp(matchedAntennaIds);
      matchedAntennaIds.resize(matchedAntennaIds.nelements() +
			     currentMatch.nelements(), True);
      matchedAntennaIds = concatenateArray(temp, currentMatch);
    };
  };
  return matchedAntennaIds;
};

//-------------------------------------------------------------------------

Vector<Int> MSAntennaIndex::matchAntennaNameAndStation(const String& name,
						       const String& station)
{
// Match a antenna and station name pair to a set of antenna id's
// Input:
//    name                         const String&  Antenna name to match
//    station                      const String&  Station name to match
// Output:
//    matchAntennaNameAndStation   Vector<Int>    Matching antenna id's
//
  LogicalArray maskArray = (msAntennaCols_p.name().getColumn()==name &&
			    msAntennaCols_p.station().getColumn()==station &&
			    !msAntennaCols_p.flagRow().getColumn());
  MaskedArray<Int> maskAntennaId(antennaIds_p, maskArray);
  return maskAntennaId.getCompressedArray();
}; 

//-------------------------------------------------------------------------

