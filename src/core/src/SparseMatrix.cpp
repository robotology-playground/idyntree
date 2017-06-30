/*
 * Copyright (C) 2016 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro, Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "SparseMatrix.h"

#include "Triplets.h"

#include <cassert>
#include <sstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cstring>
#include <iterator>

#ifndef UNUSED
#define UNUSED(x) (void)(sizeof((x), 0))
#endif

namespace iDynTree {

    SparseMatrix::SparseMatrix() : SparseMatrix(0, 0) {}

    SparseMatrix::SparseMatrix(unsigned rows, unsigned cols)
    : SparseMatrix(rows, cols, iDynTree::VectorDynSize())
    { }

    SparseMatrix::SparseMatrix(unsigned rows, unsigned cols, const iDynTree::VectorDynSize& memoryReserveDescription)
    : m_outerStarts(rows + 1, 0)
    , m_allocatedSize(0)
    , m_rows(rows)
    , m_columns(cols)
    , m_ordering(RowMajor)
    {
        initializeMatrix(rows, cols, memoryReserveDescription.data(), memoryReserveDescription.size());
    }

    void SparseMatrix::initializeMatrix(unsigned rows, unsigned cols, const double *vector, unsigned vectorSize)
    {
        UNUSED(rows);
        UNUSED(cols);

        for (unsigned i = 0; i < vectorSize; ++i) {
            m_allocatedSize += vector[i];
        }

        m_values.reserve(m_allocatedSize);

        // Inner indices has same size of values
        m_innerIndices.reserve(m_allocatedSize);
    }

    SparseMatrix::~SparseMatrix() {}

    bool SparseMatrix::valueIndex(unsigned row, unsigned col, unsigned &rowNZIndex) const
    {
        assert(row >= 0 && row < rows()
               && col >= 0 && col < columns());
        //We can use std::lower_bound to between rowNZIndex and rowNZIndex + rowNNZ
        //They are already sorted. The only critical point is if we have -1 in the matrix
        //Which right now it does not apply
        int colBegin = m_outerStarts[row];
        int colEnd = m_outerStarts[row + 1];
        std::vector<int>::const_iterator innerVectorBegin = m_innerIndices.begin();

        //initialize the return value to be the first element of the row
        rowNZIndex = colBegin;
        if (colEnd - colBegin == 0) {
            //empty row, avoid searching
            return false;
        }

        std::vector<int>::const_iterator foundIndex = std::lower_bound(innerVectorBegin + colBegin,
                                                                       innerVectorBegin + colEnd, col);

        //Compute the index of the first element next or equal to the one we
        //were looking for

        rowNZIndex = std::distance(innerVectorBegin, foundIndex);
        if (foundIndex == innerVectorBegin + colEnd) {
            //not found
            //return the index of the last element of the row
            //as we have to put the element as last element
            return false;
        }

        if (*foundIndex >= 0
            && static_cast<unsigned>(*foundIndex) == col) {
            //found
            return true;
        }

        return false;
    }

    unsigned SparseMatrix::insert(unsigned int row, unsigned int col, double value)
    {
        assert(row >= 0 && row < rows()
               && col >= 0 && col < columns());

        //first: check if there is space in the arrays
        if (m_allocatedSize <= m_values.size()) {
            reserve(m_values.size() + 10);
        }

        //find insertion position
        unsigned insertionIndex = 0;
        if (valueIndex(row, col, insertionIndex)) {
            //???: what if the element exists alredy in the matrix?
            return insertionIndex;
        }

        //I found the index. Now I have to shift to the right the values and inner elements
        m_values.resize(m_values.size() + 1);
        m_innerIndices.resize(m_innerIndices.size() + 1);

        for (unsigned i = insertionIndex; i < m_values.size() - 1; ++i) {
            m_values(i + 1) = m_values(i);
            m_innerIndices[i + 1] = m_innerIndices[i];
        }
        m_values(insertionIndex) = value;
        m_innerIndices[insertionIndex] = col;
        //update row NNZ
        for (unsigned rowIndex = row; rowIndex < rows(); ++rowIndex) {
            m_outerStarts[rowIndex + 1]++;
        }

        return insertionIndex;
    }



    unsigned SparseMatrix::numberOfNonZeros() const
    {
        return m_values.size();
    }

    unsigned SparseMatrix::nonZeroElementsForRowAtIndex(unsigned rowIndex) const
    {
        assert(rowIndex >= 0 && rowIndex < rows());
        return m_outerStarts[rowIndex + 1] - m_outerStarts[rowIndex];
    }

    void SparseMatrix::resize(unsigned rows, unsigned columns)
    {
        VectorDynSize empty; //this does not allocate memory
        resize(rows, columns, empty);
    }

    void SparseMatrix::resize(unsigned rows, unsigned columns, const iDynTree::VectorDynSize &columnNNZInformation)
    {
        //suppress unused parameter for now
        ((void)columnNNZInformation);
        //Avoid destroying the matrix if the size is the same
        if (m_rows == rows && m_columns == columns)
            return;

        m_rows = rows;
        m_columns = columns;

        m_outerStarts.resize(rows);
        m_outerStarts.assign(rows, 0);

        //TODO: use the information on the NNZ to reserve memory
    }

    void SparseMatrix::reserve(unsigned nonZeroElements)
    {
        if (nonZeroElements <= m_allocatedSize) return; //do nothing

        m_values.reserve(nonZeroElements);
        m_innerIndices.reserve(nonZeroElements);
        m_allocatedSize = nonZeroElements;

    }

    void SparseMatrix::zero()
    {
        //zero: simply clear
        m_values.resize(0);
        m_innerIndices.resize(0);
        m_outerStarts.resize(0);
    }


    void SparseMatrix::setFromConstTriplets(const iDynTree::Triplets& triplets)
    {
        iDynTree::Triplets copy(triplets);
        setFromTriplets(copy);
    }

    void SparseMatrix::setFromTriplets(iDynTree::Triplets& triplets)
    {
        if (triplets.size() == 0) return;

        //Get number of NZ and reserve buffers O(1) : size of compressed vector
        //We can overestimate with the size of triplets
        reserve(triplets.size());

        //Fastest way is to order by row and column N*log2(N)
        std::sort(triplets.begin(), triplets.end(), Triplet::rowMajorCompare);

        //now is a simple insert O(N) +
        //find to remove duplicates
        //Note: find is useless if array is sorted
        //Resize to maximum value. Will shrink at the end
        m_values.resize(triplets.size());
        m_innerIndices.resize(triplets.size());
        m_outerStarts.assign(m_rows + 1, 0); //reset vector


        unsigned lastRow = 0;
        unsigned lastColumn = 0;
        unsigned innerIndex = 0;
        unsigned lastIndex = innerIndex;
        m_values(0) = 0; //initialize the first element

        for (std::vector<Triplet>::const_iterator iterator(triplets.begin());
             iterator != triplets.end(); ++iterator) {
            // As triplets are ordered, only subsequent elements can be equal
            if (lastRow == iterator->row && lastColumn == iterator->column) {
                //Adjust for the first element
                //this should happen only the first time
                m_values(lastIndex) += iterator->value;
                //innerIndex should point to the next element
                //If the next element is at the same position, innerIndex will be ignored,
                //otherwise it will point to the next (free) element
                innerIndex = lastIndex + 1;
                continue;
            }


            //if current row is different from lastRow
            //I have to update the outerStarts vector
            if (lastRow != iterator->row) {
                for (std::vector<int>::iterator outerIt(m_outerStarts.begin() + lastRow + 1);
                     outerIt <= m_outerStarts.begin() + iterator->row; ++outerIt) {
                    *outerIt = innerIndex;
                }
                lastRow = iterator->row;
            }
            m_values(innerIndex) = iterator->value;
            m_innerIndices[innerIndex] = iterator->column;
            lastIndex = innerIndex;
            //increment index as this should always point to the next element
            ++innerIndex;
            lastColumn = iterator->column;

        }
        if (lastRow < m_rows) {
            for (std::vector<int>::iterator outerIt(m_outerStarts.begin() + lastRow + 1);
                 outerIt < m_outerStarts.end(); ++outerIt) {
                *outerIt = innerIndex;
            }

        }

        //Shrink containers
        m_values.resize(innerIndex);
        m_innerIndices.resize(innerIndex);

    }

    SparseMatrix SparseMatrix::sparseMatrixFromTriplets(unsigned rows,
                                                        unsigned cols,
                                                        const iDynTree::Triplets& nonZeroElements)
    {
        SparseMatrix newMatrix(rows, cols);
        newMatrix.setFromConstTriplets(nonZeroElements);
        return newMatrix;
    }

    unsigned SparseMatrix::rows() const { return m_rows; }
    unsigned SparseMatrix::columns() const { return m_columns; }

    double SparseMatrix::operator()(unsigned int row, unsigned int col) const
    {
        assert(row >= 0 && row < rows()
               && col >= 0 && col < columns());

        unsigned index = 0;
        double value = 0;
        if (valueIndex(row, col, index)) {
            value = m_values(index);
        }
        return value;
    }

    double& SparseMatrix::operator()(unsigned int row, unsigned int col)
    {
        assert(row >= 0 && row < rows()
               && col >= 0 && col < columns());

        unsigned index = 0;
        if (valueIndex(row, col, index)) {
            return m_values(index);
        } else {
            return m_values(insert(row, col, 0));
        }
    }

    double * SparseMatrix::valuesBuffer() { return m_values.data(); }

    double const * SparseMatrix::valuesBuffer() const { return m_values.data(); }

    int * SparseMatrix::innerIndicesBuffer()
    {
        return m_innerIndices.data();
    }

    int const * SparseMatrix::innerIndicesBuffer() const
    {
        return m_innerIndices.data();
    }

    int * SparseMatrix::outerIndicesBuffer()
    {
        return m_outerStarts.data();
    }

    int const * SparseMatrix::outerIndicesBuffer() const
    {
        return m_outerStarts.data();
    }

    // Deprecated
    int * SparseMatrix::innerIndecesBuffer()
    {
        return innerIndicesBuffer();
    }
    // Deprecated
    int const * SparseMatrix::innerIndecesBuffer() const
    {
        return innerIndicesBuffer();
    }
    // Deprecated
    int * SparseMatrix::outerIndecesBuffer()
    {
        return outerIndicesBuffer();
    }
    // Deprecated
    int const * SparseMatrix::outerIndecesBuffer() const
    {
        return outerIndicesBuffer();
    }

    std::string SparseMatrix::description(bool fullMatrix) const
    {
        std::ostringstream stream;
        if (!fullMatrix) {
            for (const_iterator it = begin(); it != end(); ++it) {
                stream << it->value << "(" << it->row << ", " << it->column << ") ";
            }
        } else {
            for (unsigned row = 0; row < rows(); ++row) {
                for (unsigned col = 0; col < columns(); ++col) {
                    unsigned index = 0;
                    if (!valueIndex(row, col, index)) {
                        stream << "0 ";
                    } else {
                        stream << m_values(index) << " ";
                    }
                }
                stream << std::endl;
            }

        }
        return stream.str();
    }

#ifndef NDEBUG
    std::string SparseMatrix::internalDescription() const
    {
        std::ostringstream stream;
        stream << "Values: \n";
        for (unsigned i = 0; i < m_values.size(); ++i) {
            stream << m_values(i) << " ";
        }
        stream << "\nInner indices: \n";
        for (unsigned i = 0; i < m_values.size(); ++i) {
            stream << m_innerIndices[i] << " ";
        }
        stream << "\nOuter indices: \n";
        for (unsigned i = 0; i < m_rows + 1; ++i) {
            stream << m_outerStarts[i] << " ";
        }
        stream << "\n";
        return stream.str();
    }
#endif

    SparseMatrix::iterator SparseMatrix::begin()
    {
        return iterator(*this);
    }

    SparseMatrix::const_iterator SparseMatrix::begin() const
    {
        return const_iterator(*this);
    }

    SparseMatrix::iterator SparseMatrix::end()
    {
        iterator it(*this, false);
        (&it)->m_index = -1;
        return it;
    }

    SparseMatrix::const_iterator SparseMatrix::end() const
    {
        const_iterator it(*this, false);
        (&it)->m_index = -1;
        return it;
    }

    void SparseMatrix::convertToColumnMajor(double *values, int *innerBuffer, int *outerBuffer) const
    {
        //empty matrix
        if (m_values.size() == 0) return;

        //assume buffers are allocated correctly
        //reset outer buffer
        std::memset(outerBuffer, 0, sizeof(int) * (m_columns + 1));

        //Insert each element by looking for the correct position
        //sorted insertion => O(N logN)
        //This is equivalent to a sort :)
        //The only think is that we sort a temporary array as we have to keep track
        //of the values assosiated with the column
        std::vector<Triplet> colMajorInnerContainer;
        colMajorInnerContainer.reserve(m_values.size());

        for (const_iterator it(begin()); it != end(); ++it) {
            colMajorInnerContainer.push_back(Triplet(it->row, it->column, it->value));
        }

        std::sort(colMajorInnerContainer.begin(), colMajorInnerContainer.end(), Triplet::columnMajorCompare);

        //Now we iterate on the vector and fill the outputs
        //We can directly fill the values and the row index
        std::vector<Triplet>::iterator it(colMajorInnerContainer.begin());
        std::vector<Triplet>::iterator begin = it;
        unsigned currentColumn = begin->column;
        unsigned currentNNZ = 0;
        for (; it != colMajorInnerContainer.end(); ++it) {
            unsigned index = std::distance(begin, it);
            currentNNZ++;

            if (currentColumn != it->column) {
                //this column is different from the previous one
                //Copy the current NNZ to all the columns till the new one
                //And start counting again the NNZ
                for (unsigned col = currentColumn + 1; col <= it->column; ++col) {
                    outerBuffer[col] = currentNNZ-1;
                }
                currentColumn = it->column;
            }

            values[index] = it->value;
            innerBuffer[index] = it->row;
        }
        //update last value
        outerBuffer[columns()] = currentNNZ;
    }

    void SparseMatrix::convertFromColumnMajor(unsigned rows,
                                              unsigned columns,
                                              unsigned numberOfNonZeros,
                                              double * const values,
                                              int * const innerBuffer,
                                              int * const outerBuffer)
    {
        //reset size
        resize(rows, columns);

        //iterate over all values and fill a triplet list
        Triplets triplets;
        triplets.reserve(numberOfNonZeros);

        for (unsigned col = 0; col < columns; ++col) {
            //iterate over the rows nz
            for (int innerIndex = outerBuffer[col]; innerIndex < outerBuffer[col + 1]; ++innerIndex) {
                triplets.pushTriplet(Triplet(innerBuffer[innerIndex], col, values[innerIndex]));
            }
        }

        setFromTriplets(triplets);

    }

    // Iterator implementation

    SparseMatrix::Iterator::TripletRef::TripletRef(unsigned row, unsigned column, double *value)
    : m_row(row)
    , m_column(column)
    , m_value(value) {}

    SparseMatrix::Iterator::Iterator(iDynTree::SparseMatrix &matrix, bool valid)
    : m_matrix(matrix)
    , m_index(-1)
    , m_currentTriplet(-1, -1, 0)
    , m_nonZerosInRow(1) //to initialize the counter
    {
        if (matrix.m_values.size() == 0 || !valid) return;
        m_index = 0;

        updateTriplet();
    }

    void SparseMatrix::Iterator::updateTriplet()
    {
        m_currentTriplet.m_value = &(m_matrix.m_values(m_index));
        m_currentTriplet.m_column = m_matrix.m_innerIndices[m_index];

        if (--m_nonZerosInRow <= 0) {
            //increment row
            m_currentTriplet.m_row++;
            while (static_cast<unsigned>(m_currentTriplet.m_row) < m_matrix.rows()) {
                //compute row NNZ
                m_nonZerosInRow = m_matrix.m_outerStarts[m_currentTriplet.m_row + 1]
                - m_matrix.m_outerStarts[m_currentTriplet.m_row];
                if (m_nonZerosInRow > 0)
                    break;

                //increment row
                m_currentTriplet.m_row++;
            }
        }

    }

    SparseMatrix::Iterator& SparseMatrix::Iterator::operator++()
    {
        if (m_index < 0) {
            //Iterator is not valid. We do nothing
            return *this;
        }
        m_index++;
        if (static_cast<unsigned>(m_index) >= m_matrix.m_values.size()) {
            //Out of range
            m_index = -1;
        } else {
            updateTriplet();
        }

        return *this;
    }

    SparseMatrix::Iterator SparseMatrix::Iterator::operator++(int)
    {
        if (m_index < 0) {
            //Iterator is not valid. We do nothing
            return *this;
        }
        Iterator newIterator(*this);
        ++newIterator;
        return newIterator;
    }

    bool SparseMatrix::Iterator::operator==(const Iterator &it) const
    {
        return &m_matrix == &((&it)->m_matrix) //check that we are pointing to the same matrix
        && m_index == it.m_index;
    }

    bool SparseMatrix::Iterator::operator==(const ConstIterator &it) const
    {
        return &m_matrix == &((&it)->m_matrix) //check that we are pointing to the same matrix
        && m_index == it.m_index;
    }

    SparseMatrix::Iterator::reference SparseMatrix::Iterator::operator*()
    {
        return m_currentTriplet;
    }

    SparseMatrix::Iterator::pointer SparseMatrix::Iterator::operator->()
    {
        return &m_currentTriplet;
    }

    bool SparseMatrix::Iterator::isValid() const
    {
        return m_index >= 0
        && true; //TODO: check if we are < than end or >= begin
    }

    SparseMatrix::ConstIterator::ConstIterator(const iDynTree::SparseMatrix &matrix, bool valid)
    : m_matrix(matrix)
    , m_index(-1)
    , m_currentTriplet(-1, -1, 0)
    , m_nonZerosInRow(1) //to initialize the counter
    {
        if (matrix.m_values.size() == 0 || !valid) return;
        m_index = 0;

        updateTriplet();
    }

    SparseMatrix::ConstIterator::ConstIterator(const SparseMatrix::Iterator& iterator)
    : m_matrix(iterator.m_matrix)
    , m_index(iterator.m_index)
    , m_currentTriplet(iterator.m_currentTriplet.row(), iterator.m_currentTriplet.column(), iterator.isValid() ? iterator.m_currentTriplet.value() : 0)
    , m_nonZerosInRow(iterator.m_nonZerosInRow) { }

    void SparseMatrix::ConstIterator::updateTriplet()
    {
        m_currentTriplet.value = m_matrix.m_values(m_index);
        m_currentTriplet.column = m_matrix.m_innerIndices[m_index];

        if (--m_nonZerosInRow <= 0) {
            //increment row
            m_currentTriplet.row++;
            while (static_cast<unsigned>(m_currentTriplet.row) < m_matrix.rows()) {
                //compute row NNZ
                m_nonZerosInRow = m_matrix.m_outerStarts[m_currentTriplet.row + 1]
                - m_matrix.m_outerStarts[m_currentTriplet.row];
                if (m_nonZerosInRow > 0)
                    break;

                //increment row
                m_currentTriplet.row++;
            }
        }
    }

    SparseMatrix::ConstIterator& SparseMatrix::ConstIterator::operator++()
    {
        if (m_index < 0) {
            //Iterator is not valid. We do nothing
            return *this;
        }
        m_index++;
        if (static_cast<unsigned>(m_index) >= m_matrix.m_values.size()) {
            //Out of range
            m_index = -1;
        } else {
            updateTriplet();
        }

        return *this;
    }

    SparseMatrix::ConstIterator SparseMatrix::ConstIterator::operator++(int)
    {
        if (m_index < 0) {
            //Iterator is not valid. We do nothing
            return *this;
        }
        ConstIterator newIterator(*this);
        ++newIterator;
        return newIterator;
    }

    bool SparseMatrix::ConstIterator::operator==(const ConstIterator &it) const
    {
        return &m_matrix == &((&it)->m_matrix) //check that we are pointing to the same matrix
        && m_index == it.m_index;
    }

    bool SparseMatrix::ConstIterator::operator==(const Iterator &it) const
    {
        return &m_matrix == &((&it)->m_matrix) //check that we are pointing to the same matrix
        && m_index == it.m_index;
    }

    SparseMatrix::ConstIterator::reference SparseMatrix::ConstIterator::operator*()
    {
        return m_currentTriplet;
    }

    SparseMatrix::ConstIterator::pointer SparseMatrix::ConstIterator::operator->()
    {
        return &m_currentTriplet;
    }

    bool SparseMatrix::ConstIterator::isValid() const
    {
        return m_index >= 0
        && true; //TODO: check if we are < than end or >= begin
    }

}
