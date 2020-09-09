/*
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#ifndef IDYNTREE_MATRIX_VIEW_H
#define IDYNTREE_MATRIX_VIEW_H

#include <cassert>

#include <type_traits>

#include <iDynTree/Core/Span.h>
#include <iDynTree/Core/Utils.h>

// constexpr workaround for SWIG
#ifdef SWIG
#define IDYNTREE_CONSTEXPR
#else
#define IDYNTREE_CONSTEXPR constexpr
#endif


namespace iDynTree
{

    namespace MatrixViewInternal
    {
        // this is required to be compatible with c++17
        template <typename... Ts> struct make_void { typedef void type; };
        template <typename... Ts> using void_t = typename make_void<Ts...>::type;

        /**
         * has_IsRowMajor is used to build a type-dependent expression that check if an
         * element has IsRowMajor argument. This specific implementation is used when
         * the the object has not IsRowMajor.
         */
        template <typename T, typename = void>
        struct has_IsRowMajor : std::false_type {};

        /**
         * has_IsRowMajor is used to build a type-dependent expression that check if an
         * element has IsRowMajor argument. This specific implementation is used when
         * the the object has not IsRowMajor, indeed <code>void_t<\endcode> is used to
         * detect ill-formed types in SFINAE context.
         */
        template <typename T>
        struct has_IsRowMajor<T, void_t<decltype(T::IsRowMajor)>> : std::true_type {};

    } // namespace MatrixViewIntenal

    /**
     * Type of storage ordering
     */
    enum class StorageOrder
    {
        RowMajor,
        ColMajor
    };

    /**
     * MatrixView implements a view interface of Matrices. Both RowMajor and ColMajor matrices are
     * supported.
     * @note The user should define the storage ordering when the MatrixView is created (the default
     order is RowMajor). However if the MatrixView is generated:
     *   - from an object having a public member called <code>IsRowMajor</code>, the correct storage
     order is chosen.
     *   - from another MatrixView, the correct storage order is chosen.
     */
    template <class ElementType> class MatrixView
    {
    public:
        using element_type = ElementType;
        using value_type = std::remove_cv_t<ElementType>;
        using index_type = std::ptrdiff_t;
        using pointer = element_type*;
        using reference = element_type&;

    private:
        pointer m_storage;
        index_type m_rows;
        index_type m_cols;

        StorageOrder m_storageOrder;

        index_type rawIndex(index_type row, index_type col) const
        {
            if (m_storageOrder == StorageOrder::RowMajor)
            {
                return (col + this->m_cols * row);
            } else
            {
                return (this->m_rows * col + row);
            }
        }

    public:

        MatrixView()
            : MatrixView(nullptr, 0, 0, StorageOrder::RowMajor)
        {}
        MatrixView(const MatrixView& other)
            : MatrixView(other.m_storage, other.m_rows, other.m_cols, other.m_storageOrder)
        {
        }

        template <
            class OtherElementType,
            class = std::enable_if_t<
                details::is_allowed_element_type_conversion<OtherElementType, element_type>::value>>
        IDYNTREE_CONSTEXPR MatrixView(const MatrixView<OtherElementType>& other)
            : MatrixView(other.data(), other.rows(), other.cols(), other.storageOrder())
        {
        }

        template <
            class Container,
            std::enable_if_t<std::is_const<element_type>::value
                                 && std::is_convertible<decltype(std::declval<Container>().data()),
                                                        pointer>::value
                                 && MatrixViewInternal::has_IsRowMajor<Container>::value
                                 && !std::is_same<Container, MatrixView>::value,
                             int> = 0>
        MatrixView(const Container& matrix)
            : MatrixView(matrix.data(),
                         matrix.rows(),
                         matrix.cols(),
                         Container::IsRowMajor ? StorageOrder::RowMajor
                                               : StorageOrder::ColMajor)
        {
        }

        template <
            class Container,
            std::enable_if_t<std::is_const<element_type>::value
                                 && std::is_convertible<decltype(std::declval<Container>().data()),
                                                        pointer>::value
                                 && !MatrixViewInternal::has_IsRowMajor<Container>::value
                                 && !std::is_same<Container, MatrixView>::value,
                             int> = 0>
        MatrixView(const Container& matrix, const StorageOrder& order = StorageOrder::RowMajor)
            : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), order)
        {
        }

        template <class Container,
                  std::enable_if_t<
                      std::is_convertible<decltype(std::declval<Container>().data()), pointer>::value
                          && MatrixViewInternal::has_IsRowMajor<Container>::value
                          && !std::is_same<Container, MatrixView>::value,
                      int> = 0>
        MatrixView(Container& matrix)
            : MatrixView(matrix.data(),
                         matrix.rows(),
                         matrix.cols(),
                         Container::IsRowMajor ? StorageOrder::RowMajor
                                               : StorageOrder::ColMajor)
        {
        }

        template <class Container,
                  std::enable_if_t<
                      std::is_convertible<decltype(std::declval<Container>().data()), pointer>::value
                          && !MatrixViewInternal::has_IsRowMajor<Container>::value
                          && !std::is_same<Container, MatrixView>::value,
                      int> = 0>
        MatrixView(Container& matrix, const StorageOrder& order = StorageOrder::RowMajor)
            : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), order)
        {
        }

        MatrixView(pointer in_data,
                   index_type in_rows,
                   index_type in_cols,
                   const StorageOrder& order = StorageOrder::RowMajor)
            : m_storage(in_data)
            , m_rows(in_rows)
            , m_cols(in_cols)
            , m_storageOrder(order)
        {
        }

        const StorageOrder& storageOrder() const noexcept
        {
            return m_storageOrder;
        }

        pointer data() const noexcept
        {
            return m_storage;
        }

        /**
         * @name Matrix interface methods.
         * Methods exposing a matrix-like interface to MatrixView.
         *
         */
        ///@{
        reference operator()(index_type row, const index_type col) const
        {
            assert(row < this->rows());
            assert(col < this->cols());
            return this->m_storage[rawIndex(row, col)];
        }

        index_type rows() const noexcept
        {
            return this->m_rows;
        }

        index_type cols() const noexcept
        {
            return this->m_cols;
        }
        ///@}
    };

    template <class ElementType>
    IDYNTREE_CONSTEXPR MatrixView<ElementType>
    make_matrix_view(ElementType* ptr,
                     typename MatrixView<ElementType>::index_type rows,
                     typename MatrixView<ElementType>::index_type cols,
                     const StorageOrder& order = StorageOrder::RowMajor)
    {
        return MatrixView<ElementType>(ptr, rows, cols, order);
    }

    template <class Container,
              std::enable_if_t<
                  MatrixViewInternal::has_IsRowMajor<Container>::value
                      || std::is_same<MatrixView<typename Container::value_type>, Container>::value,
                  int> = 0>
    IDYNTREE_CONSTEXPR MatrixView<typename Container::value_type> make_matrix_view(Container& cont)
    {
        return MatrixView<typename Container::value_type>(cont);
    }

    template <class Container,
              std::enable_if_t<
                  MatrixViewInternal::has_IsRowMajor<Container>::value
                      || std::is_same<MatrixView<const typename Container::value_type>, Container>::value,
                  int> = 0>
    IDYNTREE_CONSTEXPR MatrixView<const typename Container::value_type> make_matrix_view(const Container& cont)
    {
        return MatrixView<const typename Container::value_type>(cont);
    }

    template <class Container,
              std::enable_if_t<
                  !MatrixViewInternal::has_IsRowMajor<Container>::value
                      && !std::is_same<MatrixView<typename Container::value_type>, Container>::value,
                  int> = 0>
    IDYNTREE_CONSTEXPR MatrixView<typename Container::value_type>
    make_matrix_view(Container& cont,
                     const StorageOrder& order = StorageOrder::RowMajor)
    {
        return MatrixView<typename Container::value_type>(cont, order);
    }

    template <class Container,
              std::enable_if_t<
                  !MatrixViewInternal::has_IsRowMajor<Container>::value
                      && !std::is_same<MatrixView<typename Container::value_type>, Container>::value,
                  int> = 0>
    IDYNTREE_CONSTEXPR MatrixView<const typename Container::value_type>
    make_matrix_view(const Container& cont,
                     const StorageOrder& order = StorageOrder::RowMajor)
    {
        return MatrixView<const typename Container::value_type>(cont, order);
    }

} // namespace iDynTree

#endif /* IDYNTREE_MATRIX_MATRIX_VIEW_H */
