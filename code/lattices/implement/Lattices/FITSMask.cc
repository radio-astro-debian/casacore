//# FITSMask.cc: an on-the-fly mask for FITS images
//# Copyright (C) 1997,1998,1999,2000,2001,2002
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


#include <trial/Lattices/FITSMask.h>

#include <aips/Arrays/Array.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/IPosition.h>
#include <aips/Mathematics/Math.h>
#include <trial/Tables/TiledFileAccess.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/DataType.h>
#include <aips/Exceptions/Error.h>



FITSMask::FITSMask (TiledFileAccess* tiledFile)
: itsTiledFilePtr(tiledFile),
  itsScale(1.0),
  itsOffset(0.0),
  itsShortMagic(0),
  itsLongMagic(0),
  itsHasIntBlanks(False)
{
   AlwaysAssert(itsTiledFilePtr->dataType()==TpFloat ||
                itsTiledFilePtr->dataType()==TpDouble,
                AipsError);
}

FITSMask::FITSMask (TiledFileAccess* tiledFile, Float scale, Float offset,
                    Short magic, Bool hasBlanks)
: itsTiledFilePtr(tiledFile),
  itsScale(scale),
  itsOffset(offset),
  itsShortMagic(magic),
  itsLongMagic(0),
  itsHasIntBlanks(hasBlanks)
{
   AlwaysAssert(itsTiledFilePtr->dataType()==TpShort, AipsError);
}

FITSMask::FITSMask (TiledFileAccess* tiledFile, Float scale, Float offset,
                    Int magic, Bool hasBlanks)
: itsTiledFilePtr(tiledFile),
  itsScale(scale),
  itsOffset(offset),
  itsShortMagic(0),
  itsLongMagic(magic),
  itsHasIntBlanks(hasBlanks)
{
   AlwaysAssert(itsTiledFilePtr->dataType()==TpInt, AipsError);
}


FITSMask::FITSMask (const FITSMask& other)
: itsTiledFilePtr(other.itsTiledFilePtr),
  itsScale(other.itsScale),
  itsOffset(other.itsOffset),
  itsShortMagic(other.itsShortMagic),
  itsLongMagic(other.itsLongMagic),
  itsHasIntBlanks(other.itsHasIntBlanks)
{}

FITSMask::~FITSMask()
{}

FITSMask& FITSMask::operator= (const FITSMask& other)
{
  if (this != &other) {
    itsTiledFilePtr = other.itsTiledFilePtr;
    itsBuffer.resize();
    itsBuffer = other.itsBuffer.copy();
    itsScale = other.itsScale;
    itsOffset = other.itsOffset;
    itsShortMagic = other.itsShortMagic;
    itsLongMagic = other.itsLongMagic;
    itsHasIntBlanks = other.itsHasIntBlanks;
  }
  return *this;
}

Lattice<Bool>* FITSMask::clone() const
{
  return new FITSMask (*this);
}

Bool FITSMask::isWritable() const
{
  return False;
}

IPosition FITSMask::shape() const
{
  return itsTiledFilePtr->shape();
}

Bool FITSMask::doGetSlice (Array<Bool>& mask, const Slicer& section)
{
   IPosition shp = section.length();
   if (!mask.shape().isEqual(shp)) mask.resize(shp);
   if (!itsBuffer.shape().isEqual(shp)) itsBuffer.resize(shp);
//
   if (itsTiledFilePtr->dataType()==TpFloat) {
      itsTiledFilePtr->get(itsBuffer, section);
   } else if (itsTiledFilePtr->dataType()==TpDouble) {
      Array<Double> tmp(shp);
      itsTiledFilePtr->get(tmp, section);
      convertArray(itsBuffer, tmp);
   } else if (itsTiledFilePtr->dataType()==TpInt) {
      itsTiledFilePtr->get(itsBuffer, section, itsScale, itsOffset, 
                           itsLongMagic, itsHasIntBlanks);
   } else if (itsTiledFilePtr->dataType()==TpShort) {
      itsTiledFilePtr->get(itsBuffer, section, itsScale, itsOffset, 
                           itsShortMagic, itsHasIntBlanks);
   }
//
   Bool deletePtrD;
   const Float* pData = itsBuffer.getStorage(deletePtrD);
   Bool deletePtrM;
   Bool* pMask = mask.getStorage(deletePtrM);
//
   for (uInt i=0; i<mask.nelements(); i++) {

// Blanked values are NaNs.

      pMask[i] = True;
      if (isNaN(pData[i])) pMask[i] = False;
   }
//
   itsBuffer.freeStorage(pData, deletePtrD);
   mask.putStorage(pMask, deletePtrM);
//
   return False;            // Not a reference
}

void FITSMask::doPutSlice (const Array<Bool>& sourceBuffer,
                           const IPosition& where, 	
                           const IPosition& stride)
{
   throw(AipsError("FITSMask object is not writable"));
}

