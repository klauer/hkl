// ============================================================================
//
// = CONTEXT
//    HKL Project
//
// = FILENAME
//    Exception.cpp
//
// = AUTHORS
//    V. Delos
//
// ============================================================================
# include "HKLException.h"

// ============================================================================
// DEPENDENCIES
// ============================================================================

// ============================================================================
// STATICs
// ============================================================================

// ============================================================================
// Error::Error
// ============================================================================
Error::Error (void)
  :  reason ("unknown"),
     desc ("unknown error"),
     origin ("unknown"),
     severity (ERR)
{

}

// ============================================================================
// Error::Error
// ============================================================================
Error::Error (const char *_reason,
	      const char *_desc,
              const char *_origin,
              int _severity)
  :  reason (_reason),
     desc (_desc),
     origin (_origin),
     severity (_severity)
{

}

// ============================================================================
// Error::Error
// ============================================================================
Error::Error (const std::string& _reason,
	      const std::string& _desc,
	      const std::string& _origin,
              int _severity)
  :  reason (_reason),
     desc (_desc),
     origin (_origin),
     severity (_severity)
{

}

// ============================================================================
// Error::Error
// ============================================================================
Error::Error (const Error& _src)
  :  reason (_src.reason),
     desc (_src.desc),
     origin (_src.origin),
     severity (_src.severity)
{

}

// ============================================================================
// Error::~Error
// ============================================================================
Error::~Error (void)
{

}

// ============================================================================
// Error::operator=
// ============================================================================
Error& Error::operator= (const Error& _src) 
{
  //- no self assign
  if (this == &_src) {
    return *this;
  }

  this->reason = _src.reason;
  this->desc = _src.desc;
  this->origin = _src.origin;
  this->severity = _src.severity;

  return *this;
}

// ============================================================================
// HKLException::HKLException
// ============================================================================
HKLException::HKLException (void) 
  : errors(0)
{
  this->push_error(Error());
}

// ============================================================================
// HKLException::HKLException
// ============================================================================
HKLException::HKLException (const char *_reason,
                      const char *_desc,
                      const char *_origin,
                      int _severity) 
  : errors(0)
{
  this->push_error(Error(_reason, _desc, _origin, _severity));
}

// ============================================================================
// HKLException::HKLException
// ============================================================================
HKLException::HKLException (const std::string& _reason,
                      const std::string& _desc,
                      const std::string& _origin,
                      int _severity) 
  : errors(0)
{
  this->push_error(_reason, _desc, _origin, _severity);
}

// ============================================================================
// HKLException::HKLException
// ============================================================================
HKLException::HKLException (const HKLException& _src) 
  : errors(0)
{
  for (unsigned int i = 0; i < _src.errors.size();  i++) {
    this->push_error(_src.errors[i]);
  }
}

// ============================================================================
// HKLException::HKLException
// ============================================================================
HKLException& HKLException::operator= (const HKLException& _src) 
{
  //- no self assign
  if (this == &_src) {
    return *this;
  }

  this->errors.clear();

  for (unsigned int i = 0; i < _src.errors.size();  i++) {
    this->push_error(_src.errors[i]);
  }

  return *this;
}

// ============================================================================
// HKLException::~HKLException
// ============================================================================
HKLException::~HKLException (void)
{
  this->errors.clear();
}


// ============================================================================
// HKLException::push_error
// ============================================================================
void HKLException::push_error (const char *_reason,
    					              const char *_desc,
					                  const char *_origin, 
                            int _severity)
{
  this->errors.push_back(Error(_reason, _desc, _origin, _severity));
}

// ============================================================================
// HKLException::push_error
// ============================================================================
void HKLException::push_error (const std::string& _reason,
    					              const std::string& _desc,
					                  const std::string& _origin, 
                            int _severity)
{
  this->errors.push_back(Error(_reason, _desc, _origin, _severity));
}

// ============================================================================
// HKLException::push_error
// ============================================================================
void HKLException::push_error (const Error& _error)
{
  this->errors.push_back(_error);
}
