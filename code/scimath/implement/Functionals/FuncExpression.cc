//# FuncExpression.cc: An expression executable as function
//# Copyright (C) 2001,2002
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

//# Includes
#include <trial/Functionals/FuncExpression.h>
#include <aips/Exceptions/Error.h>
#include <aips/Quanta/MUString.h>
#include <aips/Utilities/Regex.h>
#include <aips/Utilities/String.h>
#include <aips/Mathematics/Constants.h>
#include <aips/Mathematics/Math.h>

//# Constructors
FuncExpression::FuncExpression() :
  exd(), error_p(), code_p(), rps_p(),
  const_p(), npar_p(0), ndim_p(0),
  exec_p () {
  initState();
}

FuncExpression::FuncExpression(const String &prog) :
  exd(), error_p(), code_p(), rps_p(),
  const_p(), npar_p(0), ndim_p(0),
  exec_p () {
  initState();
  if (!create(prog)) {
    throw(AipsError(String("Illegal program string in "
			   "FuncExpression ctor:\n") +
		    error_p));
  };
}

FuncExpression::FuncExpression(const FuncExpression &other) :
  exd(other.exd), error_p(other.error_p),
  code_p(other.code_p), rps_p(other.rps_p),
  const_p(other.const_p), npar_p(other.npar_p), ndim_p(other.ndim_p),
  exec_p () {
  initState();
}

FuncExpression &FuncExpression::operator=(const FuncExpression &other) {
  if (this != &other) {
    exd = 	other.exd;
    error_p = 	other.error_p;
    code_p = 	other.code_p;
    rps_p = 	other.rps_p;
    const_p = 	other.const_p;
    npar_p = 	other.npar_p;
    ndim_p = 	other.ndim_p;
    exec_p.resize(0);
    initState();
  };
  return *this;
}


//# Member functions
Bool FuncExpression::create(const String &prog) {
  // Initialise program
  error_p = "";
  code_p.resize(0);
  rps_p.resize(0);
  initState();
  const_p.resize(0);
  MUString prg(prog);
  prg.skipBlank();
  while (!prg.eos()) {
    // Skip blank
    prg.skipBlank();
    // Compile a statement
    if (!compStmt(prg)) {
      code_p.resize(0);
      error_p += " at: \n'" + prg.get(0, prg.getPtr()) + "''" +
	prg.get() + "'";
      return False;
    };
  };
  return (setOp(exd.special()["FINISH"]));
}

Bool FuncExpression::compStmt(MUString &prg) {
  /// Only expressions now
  return (compExpr(prg));
}

Bool FuncExpression::compExpr(MUString &prg) {
  prg.skipBlank();
  String t;
  // Check unary
  t = prg.get().at(0,2);
  while (exd.unary1().find(t.at(0,1)) != exd.unary1().end() ||
	 exd.unary2().find(t) != exd.unary2().end()) {
    if (exd.unary2().find(t) != exd.unary2().end()) {
      if (!setOp(exd.unary2().find(t)->second)) return False;
      prg.skipChar();
    } else {
      if (!setOp(exd.unary1().find(t.at(0,1))->second)) return False;
    };
    prg.skipChar();
    prg.skipBlank();
    t = prg.get().at(0,2);
  };
  if (!compTerm(prg)) return False;
  // Get binary 
  prg.skipBlank();
  t = prg.get().at(0,2);
  while (!prg.eos() && 
	 (exd.binary1().find(t.at(0,1)) != exd.binary1().end() ||
	  exd.binary2().find(t) != exd.binary2().end())) {
    if (exd.binary2().find(t) != exd.binary2().end()) {
      if (!setOp(exd.binary2().find(t)->second)) return False;
      prg.skipChar();
    } else {
      if (!setOp(exd.binary1().find(t.at(0,1))->second)) return False;
    };
    prg.skipChar();
    if (!compExpr(prg)) return false;
    prg.skipBlank();
    t = prg.get().at(0,2);
  };
  return True;
}

Bool FuncExpression::compTerm(MUString &prg) {
  prg.skipBlank();
  // Get a value
  if (prg.testChar('(')) {
    prg.skipChar();
    if (!setOp(exd.special()["("])) return False;
    prg.skipBlank();
    if (!compExpr(prg)) return False;
    prg.skipBlank();
    if (!prg.tSkipChar(')')) {
      error_p = "Missing closing right parenthesis";
      return False;
    };
    if (!setOp(exd.special()[")"])) return False;
  } else if (prg.testAlpha()) {
    Regex parrx("p[0-9]*$");
    Regex argrx("x[0-9]*$");
    String t = prg.getAlphaNum();
    t.downcase();
    MUString tmu(t);
    if (exd.function().find(t) != exd.function().end()) {
      if (!setOp(exd.function().find(t)->second)) return False;
      prg.skipBlank();
      if (prg.tSkipChar('(')) {
	if (!compExpr(prg)) return False;
	prg.skipBlank();
	if (!prg.tSkipChar(')')) {
	  error_p = "No closing function paranethesis";
	  return False;
	};
      };
      if (!setOp(exd.special()[")"])) return False;
    } else if (t.matches(parrx) || t.matches(argrx)) {
      tmu.skipChar();
      uInt n = tmu.getuInt();
      prg.skipBlank();
      if (prg.tSkipChar('[')) {
	prg.skipBlank();
	uInt m = prg.getuInt();
	if (m == 0) {
	  error_p = "Illegal index for argument or parameter";
	  return False;
	};
	n += m-1;
	prg.skipBlank();
	if (!prg.tSkipChar(']')) {
	  error_p = "Missing closing bracket";
	  return False;
	};
      };
      FuncExprData::ExprOperator oper;
      if (t.matches(parrx)) {
	oper = exd.special()["PARAM"];
	if (n >= npar_p) npar_p = n+1;
      } else {
	oper = exd.special()["ARG"];
	if (n >= ndim_p) ndim_p = n+1;
      };
      oper.narg = n;
      if (!setOp(oper)) return False;
    } else {
      error_p = String("Unknown function name ") + t;
      return False;
    };
  } else if (prg.testDouble()) {
    Double d = prg.getDouble();
    if (!setVal(d)) return False;
    if (!setCode(exd.special()["CONST"])) return False;
  } else {
    error_p = "Missing value";
    return False;
  };
  return True;
}

Bool FuncExpression::setOp(FuncExprData::ExprOperator &oper) {
  // Check the work stack for priorities
  while (rps_p.size() > state_p.rpslow) {
    // High new priority or equal and left-to-right: roll up stack
    if (oper.priority < rps_p.back().priority ||
	(oper.priority == rps_p.back().priority &&
	 oper.priority < FuncExprData::RTLPRI)) {
      if (!setCode(rps_p.back())) return False;
      // Are there enough values to operate upon?
      if (state_p.nval < rps_p.back().narg) {
	error_p = "Not enough operands for operator '";
	error_p += rps_p.back().name + "'";
	return False;
      };
      state_p.nval -= rps_p.back().nresult;
      rps_p.pop_back();
    } else break;
  };
  // Add the new code
  if (oper.special == FuncExprData::SAVENV) {
    oper.state = state_p;
    state_p.nval =0;
    state_p.rpslow = rps_p.size()+1;
    state_p.argcnt = 0;
    rps_p.push_back(oper);
  } else if (oper.special == FuncExprData::FINAL) {
    if (oper.code == FuncExprData::COMMA) {
      if (rps_p.size() != state_p.rpslow || state_p.rpslow < 1 ||
	  rps_p[state_p.rpslow-1].category != FuncExprData::FUNC) {
	error_p = "Parameter comma separator not expected";
	return False;
      };
      rps_p[state_p.rpslow-1].state.argcnt += state_p.nval;
      state_p.nval = 0;
    } else if (oper.code == FuncExprData::FINISH) {
      if (rps_p.size() != state_p.rpslow ||
	  state_p.rpslow != 0 || state_p.nval != 1) {
	error_p = "Unexpected EOS";
	return False;
      };
    } else if (oper.code == FuncExprData::RPAREN) {
      if (rps_p.size() != state_p.rpslow || state_p.rpslow < 1) {
	error_p = "Right parenthesis not expected";
	return False;
      };
      if (rps_p[state_p.rpslow-1].code ==  FuncExprData::LPAREN) {
	if (state_p.nval != 1) {
	  error_p = "No value between ()";
	  return False;
	};
	state_p.nval += rps_p[state_p.rpslow-1].state.nval;
	state_p.rpslow = rps_p[state_p.rpslow-1].state.rpslow;
	rps_p.pop_back();
      } else if (rps_p[state_p.rpslow-1].category == FuncExprData::FUNC) {
	if (state_p.nval > 1) {
	  error_p = "Incorrect value stack for function evaluation";
	  return False;
	};
	rps_p[state_p.rpslow-1].state.argcnt += state_p.nval;
	if (rps_p[state_p.rpslow-1].state.argcnt <
	    rps_p[state_p.rpslow-1].narg ||
	    rps_p[state_p.rpslow-1].state.argcnt >
	    rps_p[state_p.rpslow-1].nmaxarg) {
	  error_p = "Incorrect number of arguments in function";
	  return False;
	};
	if (!setCode(rps_p[state_p.rpslow-1])) return False;
	state_p.nval = rps_p.back().state.nval + rps_p.back().nresult;
	state_p.rpslow = rps_p.back().state.rpslow;
	state_p.argcnt = 0;
	rps_p.pop_back();
      } else {
	error_p = "Right parenthesis not expected";
	return False;
      };
    } else {
      error_p = "Unexpected final code";
      return False;
    };
  } else {
    rps_p.push_back(oper);
  };
  return True;
}

Bool FuncExpression::setVal(const Double &val) {
  const_p.push_back(val);
  ++state_p.nval;
  return True;
}

Bool FuncExpression::setCode(const FuncExprData::ExprOperator &oper) {
  code_p.push_back(oper);
  return True;
}

void FuncExpression::initState() {
  state_p.rpslow = 0;
  state_p.nval = 0;
  state_p.argcnt = 0;
  npar_p = 0;
  ndim_p = 0;
}

const vector<FuncExprData::ExprOperator> &FuncExpression::getCode() const{
  return code_p;
}

Bool FuncExpression::exec(Double &res) const {
  error_p = "";
  res = Double(0);
  exec_p.resize(0);
  vector<Double>::const_iterator constp = const_p.begin();
  for (vector<FuncExprData::ExprOperator>::const_iterator pos=code_p.begin();
       pos != code_p.end(); pos++) {
    switch (pos->category) {
    case FuncExprData::UNA1:
    case FuncExprData::UNA2: {
      switch (pos->code) {
      case  FuncExprData::UNAMIN:
	exec_p.back() = -exec_p.back();
      case FuncExprData::UNAPLUS:
	break;
      default:
	error_p = String("Unknown execution code '") +
	  pos->name + "': programming error";
	break;
      }
      break;
    }

    case FuncExprData::BIN1:
    case FuncExprData::BIN2: {
      Double t;
      if (pos->narg == 2) {
	t = exec_p.back();
	exec_p.pop_back();
      };
      switch (pos->code) {
      case  FuncExprData::POW:
	exec_p.back() = pow(exec_p.back(), t);
	break;
      case  FuncExprData::ADD:
      exec_p.back() += t;
      break;
      case  FuncExprData::SUB:
	exec_p.back() -= t;
	break;
      case  FuncExprData::MUL:
	exec_p.back() *= t;
	break;
      case  FuncExprData::DIV:
	exec_p.back() /= t;
	break;
      default:
	error_p = String("Unknown execution code '") +
	  pos->name + "': programming error";
	break;
      }
      break;
    }

    case FuncExprData::SPEC: {
      switch (pos->code) {
      case  FuncExprData::CONST:
	exec_p.push_back(*(constp++));
	break;
      default:
	error_p = String("Unknown execution code '") +
	  pos->name + "': programming error";
	break;
      }
      break;
    }

    case FuncExprData::FUNC: {
      switch (pos->code) {
      case  FuncExprData::SIN:
	exec_p.back() = sin(exec_p.back());
	break;
      case  FuncExprData::COS:
	exec_p.back() = cos(exec_p.back());
	break;
      case  FuncExprData::ATAN:
	if (pos->state.argcnt == 1) {
	  exec_p.back() = atan(exec_p.back());
	  break;
	};
      case  FuncExprData::ATAN2: {
	Double t(exec_p.back());
	exec_p.pop_back();
	exec_p.back() = atan2(exec_p.back(), t);
	break; }
      case  FuncExprData::ASIN:
	exec_p.back() = asin(exec_p.back());
	break;
      case  FuncExprData::ACOS:
	exec_p.back() = acos(exec_p.back());
	break;
      case  FuncExprData::EXP:
	exec_p.back() = exp(exec_p.back());
	break;
      case  FuncExprData::EXP2:
	exec_p.back() = exp(exec_p.back()*C::ln2);
	break;
      case  FuncExprData::EXP10:
	exec_p.back() = exp(exec_p.back()*C::ln10);
	break;
      case  FuncExprData::LOG:
	exec_p.back() = log(exec_p.back());
	break;
      case  FuncExprData::LOG2:
	exec_p.back() = log(exec_p.back())/C::ln2;
	break;
      case  FuncExprData::LOG10:
	exec_p.back() = log10(exec_p.back());
	break;
      case  FuncExprData::PI: {
	if (pos->state.argcnt == 0) exec_p.push_back(C::pi);
	else exec_p.back() *= C::pi;
	break; }
      case  FuncExprData::EE: {
	if (pos->state.argcnt == 0) exec_p.push_back(C::e);
	else exec_p.back() *= C::e;
	break; }
      case  FuncExprData::ABS:
	exec_p.back() = abs(exec_p.back());
	break;
      case  FuncExprData::FLOOR:
	exec_p.back() = floor(exec_p.back());
	break;
      case  FuncExprData::CEIL:
	exec_p.back() = ceil(exec_p.back());
	break;
      case  FuncExprData::ROUND:
	exec_p.back() = floor(exec_p.back()+Double(0.5));
	break;
      case  FuncExprData::INT:
	if (exec_p.back() < 0) exec_p.back() = floor(exec_p.back());
	else exec_p.back() = ceil(exec_p.back());
	break;
      case  FuncExprData::FRACT:
	if (exec_p.back() < 0) exec_p.back() -= ceil(exec_p.back());
	else exec_p.back() -= floor(exec_p.back());
	break;
      case  FuncExprData::SQRT:
	exec_p.back() = sqrt(exec_p.back());
	break;
      case  FuncExprData::REAL:
	break;
      case  FuncExprData::IMAG:
	exec_p.back() = Double(0);
	break;
      case  FuncExprData::AMPL:
	break;
      case  FuncExprData::PHASE:
	exec_p.back() = Double(0);
	break;
      default:
	error_p = String("Unknown execution code '") +
	  pos->name + "': programming error";
	break;
      }
      break;
    }

    default:
      error_p = String("Unknown execution code '") +
	pos->name + "': programming error";
      break;
    }
  };
  if (exec_p.size() != 1 && error_p.empty()) error_p = "No value returned";
  if (error_p.empty()) {
    res = exec_p.back();
    return True;
  };
  return False;
}

void FuncExpression::print(ostream &os) const {
  for (vector<FuncExprData::ExprOperator>::const_iterator pos=code_p.begin();
       pos != code_p.end(); pos++) exd.print(os, *pos);
}

//# Global functions
ostream &operator<<(ostream &os, const FuncExpression &ed) {
  ed.print(os);
  return os;
}

//# Templates (for test purposes)
template class vector<FuncExprData::ExprOperator>;
template class vector<Double>;

template
FuncExprData::ExprOperator 
*__uninitialized_copy_aux<FuncExprData::ExprOperator *,
  FuncExprData::ExprOperator *>(FuncExprData::ExprOperator *,
				FuncExprData::ExprOperator *,
				FuncExprData::ExprOperator *,
				__false_type);
template
FuncExprData::ExprOperator 
*__uninitialized_copy_aux<FuncExprData::ExprOperator const *,
  FuncExprData::ExprOperator *>(FuncExprData::ExprOperator const *,
				FuncExprData::ExprOperator const *,
				FuncExprData::ExprOperator *,
				__false_type);
template
FuncExprData::ExprOperator 
*fill_n<FuncExprData::ExprOperator *, unsigned int,
  FuncExprData::ExprOperator>(FuncExprData::ExprOperator *,
			      unsigned int,
			      FuncExprData::ExprOperator const &);
template
double
*fill_n<double *, unsigned int,
  double>(double *, unsigned int, double const &);
template
void fill<FuncExprData::ExprOperator *,
  FuncExprData::ExprOperator>(FuncExprData::ExprOperator *,
			      FuncExprData::ExprOperator *,
			      FuncExprData::ExprOperator const &);
template
void fill<double *,
  double>(double *, double *, double const &);
template
FuncExprData::ExprOperator 
*__uninitialized_fill_n_aux<FuncExprData::ExprOperator *, unsigned int,
  FuncExprData::ExprOperator>(FuncExprData::ExprOperator *, unsigned int,
			      FuncExprData::ExprOperator const &,
			      __false_type);

