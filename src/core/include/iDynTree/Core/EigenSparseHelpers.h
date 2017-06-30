/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef IDYNTREE_EIGEN_SPARSE_HELPERS_H
#define IDYNTREE_EIGEN_SPARSE_HELPERS_H

#include <Eigen/SparseCore>
#include <iDynTree/Core/SparseMatrix.h>

namespace iDynTree
{

//SparseMatrix helpers
inline Eigen::Map< Eigen::SparseMatrix<double, Eigen::RowMajor> > toEigen(iDynTree::SparseMatrix & mat)
{
    return Eigen::Map<Eigen::SparseMatrix<double, Eigen::RowMajor> >(mat.rows(),
                                                                     mat.columns(),
                                                                     mat.numberOfNonZeros(),
                                                                     mat.outerIndicesBuffer(),
                                                                     mat.innerIndicesBuffer(),
                                                                     mat.valuesBuffer(),
                                                                     0); //compressed format
}

inline Eigen::Map<const Eigen::SparseMatrix<double, Eigen::RowMajor> > toEigen(const iDynTree::SparseMatrix & mat)
{
    return Eigen::Map<const Eigen::SparseMatrix<double, Eigen::RowMajor> >(mat.rows(),
                                                                           mat.columns(),
                                                                           mat.numberOfNonZeros(),
                                                                           mat.outerIndicesBuffer(),
                                                                           mat.innerIndicesBuffer(),
                                                                           mat.valuesBuffer(),
                                                                           0); //compressed format
}

}


#endif /* IDYNTREE_EIGEN_SPARSE_HELPERS_H */
