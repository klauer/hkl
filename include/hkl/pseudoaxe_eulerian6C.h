#ifndef _PSEUDOAXE_EULERIAN6C_H_
#define _PSEUDOAXE_EULERIAN6C_H_

#include "pseudoaxe.h"
#include "geometry_eulerian6C.h"
#include "pseudoaxe_eulerian4C.h"

using namespace std;

namespace hkl {
    namespace pseudoAxe {
        namespace eulerian6C {

            class Tth : public PseudoAxe<geometry::Eulerian6C>
            {
            public:

              Tth(geometry::Eulerian6C &); //!< Default constructor.

              Tth(Tth const &); //!< Copy constructor

              virtual ~Tth(void); //!< Default destructor.

              void initialize(void) throw (HKLException);

              bool get_isValid(void) const;

              double get_value(void) const throw (HKLException);

              void set_value(double const &) throw (HKLException);

              /*!
               * \brief Save the pseudoaxe::Eulerian4C into a stream.
               * \param flux the stream to save the pseudoaxe::Eulerian4C into.
               * \return The stream with the pseudoaxe::Eulerian4C.
               */
              ostream & toStream(ostream &) const;

              /*!
               * \brief Restore a pseudoaxe::Eulerian4C from a stream.
               * \param flux The stream containing the pseudoaxe::Eulerian4C.
               * \return The modified stream.
               */
              istream & fromStream(istream &);

            private:
              svector m_axe;
            };

            class Q : public PseudoAxe<geometry::Eulerian6C>
            {
            public:

              Q(geometry::Eulerian6C &); //!< Default constructor.

              virtual ~Q(void); //!< Default destructor.

              void initialize(void) throw (HKLException);

              bool get_isValid(void) const;

              double get_value(void) const throw (HKLException);

              void set_value(double const &) throw (HKLException);

            private:

              mutable pseudoAxe::eulerian6C::Tth * m_tth;
            };

            namespace eulerian4C {
                namespace vertical {

                    typedef DerivedPseudoAxe<pseudoAxe::eulerian4C::vertical::Psi, geometry::Eulerian6C> Psi;

                } // namespace vertical
            } // namespace eulerian4C
        } // namespace eulerian6C.
    } // namespace pseudoAxe.
} // namespace hkl.

#endif // _PSEUDOAXE_EULERIAN6C_H_