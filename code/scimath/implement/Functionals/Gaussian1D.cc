//# NQGaussian1D.cc: A one-dimensional Gaussian class
//# Copyright (C) 1994,1995,1996,1999,2001,2002
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

//# Includes
#include <aips/Functionals/Gaussian1D.h>
#include <aips/Mathematics/Math.h>

//# Constructors

//# Operators
template<class T>
T NQGaussian1D<T>::eval(typename NQFunction1D<T>::FunctionArg x) const {
  T value = (x[0] - param_p[CENTER])/param_p[WIDTH]/fwhm2int;
  return param_p[HEIGHT] * exp(-(value*value));
}

//# Member functions
