//# SpectralFit.h: Least Squares fitting of spectral elements to spectrum
//# Copyright (C) 2001
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
//#
//# $Id$

#if !defined(TRIAL_SPECTRALFIT_H)
#define TRIAL_SPECTRALFIT_H

//# Includes
#include <aips/aips.h>

//# Forward Declarations
class SpectralElement;
template <class T> class Vector;

// <summary>
// Least Squares fitting of spectral elements to spectrum
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <ahlink module=Functionals>Functionals</ahlink> module
//   <li> <ahlink class=SpectralElement>SpectralElement</ahlink> class
// </prerequisite>
//
// <etymology>
// From spectral line and fitting
// </etymology>
//
// <synopsis>
// The SpectralFit class will do a non-linear least squares solution
// for a number of simultaneous spectral components. The initial guess
// of the elements is given in a set of SpectralElements. The final solution
// is returned in the same set.
//
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have a contained fitting of spectral profiles to an observed spectrum
// </motivation>
//
// <todo asof="2001/02/04">
//   <li> add more profile types
// </todo>

class SpectralFit {
 public:

  //# Constructors
  // Default constructor creates a default fitter without elements
  SpectralFit();
  // Construct for the given number fo elements
  explicit SpectralFit(uInt n);
  // Copy constructor (deep copy)
  SpectralFit(const SpectralFit &other);
  // Destructor
  ~SpectralFit();

  //# Operators
  // Assignment (copy semantics)
  SpectralFit &operator=(const SpectralFit &other);

  //# Member functions
  // Set an element to be fitted
  // <thrown>
  //   <li> AipsError if index too large
  // </thrown>
  void setFitElement(uInt index, SpectralElement &elem);

  // Add an element to be fitted
  void addFitElement(SpectralElement &elem);

  // Get number of elements being fitted
  uInt getNElements() const { return n_p; }

  // Get a fitted element
  // <thrown>
  //   <li> AipsError if index too large
  // </thrown>
  const SpectralElement &getElement(uInt index) const;
  // Fit the (gaussian) elements as given by the specified spectral elements
  // at the frequencies x with values y. Weights of all points are equal.
  // <group>
  Bool SpectralFit::fit(const Vector<Double> &x,
			const Vector<Double> &y);
  Bool SpectralFit::fit(const Vector<Float> &x,
			const Vector<Float> &y);
  // </group>

 private:
  //#Data
  // Number of elements to fit
  uInt n_p;
  // Capacity of fit list
  uInt cap_p;
  // Elements to be fitted
  SpectralElement *el_p;

  //# Member functions
  // Increase capacity
  void capacity();
};

#endif


