#ifndef _DERIVEDPSEUDOAXE_H_
#define _DERIVEDPSEUDOAXE_H_

#include <iostream>

#include "geometry.h"
#include "HKLException.h"
#include "pseudoaxe.h"

using namespace std;

namespace hkl {

    /*!
     * \brief A class design to describe a pseudoaxe from a geometry type
     */
    template<typename T, typename C>
    class DerivedPseudoAxe : public PseudoAxe<C>
      {      
      public:

        DerivedPseudoAxe(C &); //!< The default constructor - protected to make sure this class is abstract.

        DerivedPseudoAxe(DerivedPseudoAxe const & derivedPseudoAxe); //!<The default copy constructor.

        virtual ~DerivedPseudoAxe(void); //!< The default destructor.

        /**
         * \brief Initialize the PseudoAxe from the Geometry.
         * \param geometry The configuration to save for calculation.
         */
        void initialize(void) throw (HKLException);

        /** 
         * @brief Is a PseudoAxe valid ?
         * 
         * @return The validity of the PseudoAxe for the current Geometry.
         *
         * A pseudoAxe is valid when its value can be compute and when the meaning
         * of this value is coherant with the initialization of the pseudoAxe.
         */
        bool get_isValid(void) const;

        /**
         * \brief get the current value of the PseudoAxe.
         * \param geometry the Geometry containing the real Axe
         * \return the position of the PseudoAxe.
         */
        double get_value(void) const throw (HKLException);

        /**
         * \brief set the current value of the PseudoAxe.
         * \param geometry the Geometry containing the real Axe
         * \param value The value to set.
         * \throw HKLException if the pseudoAxe is not ready to be set.
         */
        void set_value(double const & value) throw (HKLException);

        ostream & printToStream(ostream & flux) const;

        ostream & toStream(ostream & flux) const;

        istream & fromStream(istream & flux);

      private:
        mutable typename T::value_type m_gconv; //!< The geometry used to do the conversion.
        mutable T * m_pseudoAxe; //!< Geometry used to store the initialisation of the pseudoAxe.

      };

    template<typename T, typename C>
    DerivedPseudoAxe<T, C>::DerivedPseudoAxe(C & geometry) :
      PseudoAxe<C>(geometry)
    {
      m_pseudoAxe = new T(m_gconv);
      set_name(m_pseudoAxe->get_name());
      set_description(m_pseudoAxe->get_description());
      set_valueList(m_pseudoAxe->get_valueList());
    }

    template<typename T, typename C>
    DerivedPseudoAxe<T, C>::DerivedPseudoAxe(DerivedPseudoAxe const & derivedPseudoAxe) :
      PseudoAxe<C>(derivedPseudoAxe),
      m_gconv(derivedPseudoAxe.m_gconv),
      m_pseudoAxe(derivedPseudoAxe.m_pseudoAxe)
    {
      // for now if we made a copy seg fault due to try to delete two times the same pointer m_pseudoAxe.
    }

    template<typename T, typename C>
    DerivedPseudoAxe<T, C>::~DerivedPseudoAxe(void)
      {
        delete m_pseudoAxe;
      }

    template<typename T, typename C>
    void
    DerivedPseudoAxe<T, C>::initialize(void) throw (HKLException)
      {
        m_gconv.setFromGeometry(PseudoAxe<C>::m_geometry, false);
        m_pseudoAxe->set_valueList(PseudoAxe<C>::get_valueList());
        m_pseudoAxe->initialize();
      }

    template<typename T, typename C>
    bool
    DerivedPseudoAxe<T, C>::get_isValid(void) const
      {
        m_gconv.setFromGeometry(PseudoAxe<C>::m_geometry, false);
        m_pseudoAxe->set_valueList(PseudoAxe<C>::get_valueList());
        return m_pseudoAxe->get_isValid();
      }

    template<typename T, typename C>
    double
    DerivedPseudoAxe<T, C>::get_value(void) const throw (HKLException)
      {
        m_gconv.setFromGeometry(PseudoAxe<C>::m_geometry, false);
        m_pseudoAxe->set_valueList(PseudoAxe<C>::get_valueList());
        return m_pseudoAxe->get_value();
      }

    template<typename T, typename C>
    void
    DerivedPseudoAxe<T, C>::set_value(double const & value) throw (HKLException)
      {
        m_gconv.setFromGeometry(PseudoAxe<C>::m_geometry, false);
        m_pseudoAxe->set_valueList(PseudoAxe<C>::get_valueList());
        m_pseudoAxe->set_value(value);
        PseudoAxe<C>::m_geometry.setFromGeometry(m_gconv, false);
      }

    template<typename T, typename C>
    ostream &
    DerivedPseudoAxe<T, C>::printToStream(ostream & flux) const
      {
        flux << m_pseudoAxe;
        return flux;
      }

    template<typename T, typename C>
    ostream &
    DerivedPseudoAxe<T, C>::toStream(ostream & flux) const
      {
        m_pseudoAxe->toStream(flux);
        return flux;
      }

    template<typename T, typename C>
    istream &
    DerivedPseudoAxe<T, C>::fromStream(istream & flux)
      {
        m_pseudoAxe->fromStream(flux);
        return flux;
      }

} // namespace hkl

/*!
 * \brief Overload of the << operator for the PseudoAxe class
 */
template<typename T, typename C>
ostream &
operator<<(ostream & flux, hkl::DerivedPseudoAxe<T, C> const & derivedPseudoAxe)
{
    return derivedPseudoAxe.printToStream(flux);
}



#endif // _DERIVEDPSEUDOAXE_H_