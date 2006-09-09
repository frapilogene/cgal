// Copyright (c) 1997-2001  ETH Zurich (Switzerland).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Kaspar Fischer <fischerk@inf.ethz.ch>
//                 Bernd Gaertner <gaertner@inf.ethz.ch>
#include <CGAL/basic.h>
#include <iostream>
#include <sstream>
#include <cstdlib>

#ifndef CGAL_USE_GMP
#include <CGAL/MP_Float.h>
#else
#include <CGAL/Gmpz.h>
#endif 

#include <CGAL/QP_solver.h>
#include <CGAL/QP_solver/QP_full_exact_pricing.h>
#include <CGAL/QP_solver/QP_partial_exact_pricing.h>
#include <CGAL/QP_solver/QP_full_filtered_pricing.h>
#include <CGAL/QP_solver/QP_partial_filtered_pricing.h>

#include <CGAL/QP_models.h>

struct Tags {
  typedef CGAL::Tag_false Is_linear; 
  typedef CGAL::Tag_false Is_symmetric;
  typedef CGAL::Tag_false Has_equalities_only_and_full_rank; 
  typedef CGAL::Tag_false Is_in_standard_form; 
};

int main(const int argNr,const char **args) {
  using std::cout;
  using std::endl;

  // get desired level of additional logging output:
  const int verbosity = argNr < 2? 1 : std::atoi(args[1]);

  // construct QP instance:
  typedef int IT;

#ifndef CGAL_USE_GMP 
  typedef CGAL::MP_Float ET; 
#else
  typedef CGAL::Gmpz ET;
#endif

  typedef CGAL::QP_from_mps<IT> QP;
  QP qp(std::cin,true,verbosity);

  // check for format errors in MPS file:
  if (!qp.is_valid()) {
    cout << "Input is not a valid MPS file." << endl
         << "Error: " << qp.error() << endl;
    std::exit(2);
  }

  if (verbosity > 0) {
    cout << endl << qp << endl;
  }

  // in case of an LP, zero the D matrix:
  // (Note: if you know in advance that the problem is an LP
  // you should not do this, but set Is_linear to Tag_true.)
  if (qp.is_linear() && !check_tag(Tags::Is_linear()))
    qp.make_zero_D();  

  typedef CGAL::QP_solver<QP, ET, Tags> Solver;
  Solver solver(qp, 0, verbosity);

  if (solver.is_valid()) {
    cout << "Solution is valid." << endl;
  } else {
    cout << "Solution is not valid!" << endl;
    return 1;
  }

  // get solution:
  if (solver.status() == Solver::OPTIMAL) {
    // output solution:
    cout << "Objective function value: " << 
      CGAL::to_double(solver.solution()) << endl;     
     
    cout << "Variable values:" << endl;
    Solver::Variable_value_iterator it 
      = solver.variables_value_begin() ;
    for (unsigned int i=0; i < qp.n(); ++it, ++i)
      cout << "  " << qp.name_of_variable(i) << " = "
	   << CGAL::to_double(*it) << endl;
    return 0;
  }
  if  (solver.status() == Solver::INFEASIBLE)
    cout << "Problem is infeasible." << endl;
  else // unbounded
    cout << "Problem is unbounded." << endl;
  return 0;
}
