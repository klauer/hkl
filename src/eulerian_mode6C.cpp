#include <math.h>
#include "mode.h"
#include "svecmat.h"
#include "constants.h"
#include "angleconfig.h"
#include "HKLException.h"


//************************************
//*** 4C BISECTOR HORIZONTAL MODE ***
//**********************************
eulerian_horizontal4CBissectorMode6C::eulerian_horizontal4CBissectorMode6C()
{}

eulerian_horizontal4CBissectorMode6C::~eulerian_horizontal4CBissectorMode6C()
{}

// Solving equation (11) from :
// H. You "Angle calculations for a `4S+2D' six-circle diffractometer" (1999)
// J. Appl. Cryst., 32, 614-623.
// Z11 * hphi1 + Z12 * hphi2 + Z13 * hphi3 = k*sin(delta)
// Z21 * hphi1 + Z22 * hphi2 + Z23 * hphi3 = k*(cos(delta)*cos(nu)-1.)
// Z31 * hphi1 + Z32 * hphi2 + Z33 * hphi3 = k*cos(delta)*sin(nu)
// where k = tau/lambda = q/2sin(theta) and
// eta = delta = 0.
//
// hphi1 = -q*sin(chi)*cos(phi)
// hphi2 = -q*sin(chi)*sin(phi))
// hphi3 =  q*cos(chi)
//
// If nu is constant :
// chi = arccos(hphi3 / q)
// sin(phi) = -hphi2 / (q*sin(chi))
// cos(phi) = -hphi1 / (q*sin(chi))
angleConfiguration*
  eulerian_horizontal4CBissectorMode6C::computeAngles(
  double h, double k, double l, const smatrix& UB, double lambda) const
  throw (HKLException)
{
  // h(theta) = R.hphi
  double mu;
  double nu;
  double eta;
  double chi;
  double phi;
  double theta;
  double delta;
  double sin_theta;
  double hphi_length;
  svector hphi(h,k,l);
  svector hphi_unitVector;
  eulerian_angleConfiguration6C* ac6C = 0;


  hphi.multiplyOnTheLeft(UB);
  hphi.unitVector(hphi_unitVector, hphi_length);

  if (fabs(lambda) < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "lamdba is null",
      "The wave length has not been set",
      "eulerian_horizontal4CBissectorMode6C::computeAngles()");

  if ((fabs(h) < mathematicalConstants::getEpsilon1()) && 
      (fabs(k) < mathematicalConstants::getEpsilon1()) &&
      (fabs(l) < mathematicalConstants::getEpsilon1()))
  {
    throw HKLException(
      "(h,k,l) is null",
      "check your parameters",
      "eulerian_horizontal4CBissectorMode6C::computeAngles()");
  }

  if (hphi.norminf() < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "hphi is null",
      "The matrix U has been computed from two parallel reflections or the crystal matrix is null",
      "eulerian_horizontal4CBissectorMode6C::computeAngles()");

  /////////////////////
  // Bragg relation //
  ///////////////////
  // sin(theta) = || q || * lambda * 0.5 / tau.
  sin_theta = hphi_length * lambda * 0.5;
  // We have to be consistent with the conventions 
  // previously defined when we computed the crystal 
  // reciprocal lattice.
  sin_theta = sin_theta / physicalConstants::getTau();

  if (fabs(sin_theta) > 1.)
    throw HKLException(
      "sine bigger than 1.",
      "hphi_length too big, maybe error in UB matrix",
      "eulerian_horizontal4CBissectorMode6C::computeAngles()");

  theta = asin(sin_theta);

  nu = 2.*theta;

  // By definition in horizontal bisector mode.
  delta = 0.;
  eta = 0.;
  mu = 0.;

  double so = sin(eta);
  double co = cos(eta);

  double sx = hphi.get_Z() / (hphi_length * co);
  // SPEC : without hphi_length
  //double sx = hphi.get_Z() / (co);

  if (fabs(sx) > 1.)
    throw HKLException(
      "sine bigger than 1.",
      "hphi.getZ() too big or hphi_length too small, maybe error in UB matrix",
      "eulerian_horizontal4CBissectorMode6C::computeAngles()");

  chi = acos(sx);
  // asin() returns values between -PI/2. and PI/2.
  // hphi 1st component sign tells whether chi belongs or not to
  // the other half of the circle i.e. between PI/2. and 3PI/2.
  if (hphi.get_X() < -mathematicalConstants::getEpsilon1())
    chi = 3.141592654 - chi;

  double c_phi = -hphi.get_X()*sin(chi);
  double s_phi = -hphi.get_Y()*sin(chi);

  if ((fabs(c_phi) < mathematicalConstants::getEpsilon1()) && 
      (fabs(s_phi) < mathematicalConstants::getEpsilon1()))
    phi = 0.;
  else
    phi = atan2(s_phi,c_phi);

  mu = theta;

  ac6C = new eulerian_angleConfiguration6C;
  ac6C->setDelta(delta);
  ac6C->setEta(eta);
  ac6C->setChi(chi);
  ac6C->setPhi(phi);
  ac6C->setNu(nu);
  ac6C->setMu(mu);

  return ac6C;
}

// Compute (h,k,l) from a sample of angles.
// Solve a linear system Ax = b where A is the product of the rotation matrices 
// MU, ETA, CHI, PHI by the orientation matrix U and the crystal matrix B. b is the
// scattering vector is : k(sin(delta), cos(delta).cos(nu)-1., cos(delta).sin(nu))
// and x = (h,k,l) and k = tau/lambda = q/2sin(theta). Raise an exception when det(A)=0.
void eulerian_horizontal4CBissectorMode6C::computeHKL(
  double& h, double& k, double& l, const smatrix& UB, double lambda, angleConfiguration* ac) const
  throw (HKLException)
{
  double nu    = ((eulerian_angleConfiguration6C*)ac)->getNu();
  double mu    = ((eulerian_angleConfiguration6C*)ac)->getMu();
  double eta   = ((eulerian_angleConfiguration6C*)ac)->getEta();
  double chi   = ((eulerian_angleConfiguration6C*)ac)->getChi();
  double phi   = ((eulerian_angleConfiguration6C*)ac)->getPhi();
  double delta = ((eulerian_angleConfiguration6C*)ac)->getDelta();

  double cos_nu    = cos(nu);
  double sin_nu    = sin(nu);
  double cos_mu    = cos(mu);
  double sin_mu    = sin(mu);
  double cos_eta   = cos(eta);
  double sin_eta   = sin(eta);
  double cos_chi   = cos(chi);
  double sin_chi   = sin(chi);
  double cos_phi   = cos(phi);
  double sin_phi   = sin(phi);
  double cos_delta = cos(delta);
  double sin_delta = sin(delta);

  smatrix NU;
  smatrix MU;
  smatrix ETA;
  smatrix CHI;
  smatrix PHI;
  smatrix DELTA;

  // Matrix Mu
  //  |  1.     0.       0. |
  //  |  0.  cos_mu  -sin_mu|
  //  |  0.  sin_mu   cos_mu|
  MU.set(
        1.,    0.,      0.,
        0., cos_mu, -sin_mu,
        0., sin_mu,  cos_mu);

  // Matrix Eta
  //  |  cos_eta  sin_eta 0. |
  //  | -sin_eta  cos_eta 0. |
  //  |     0.       0.   1. |
  ETA.set(
     cos_eta, sin_eta, 0.,
    -sin_eta, cos_eta, 0.,
        0.,      0.,   1.);

  // Matrix Chi
  //  |  cos_chi  0.  sin_chi |
  //  |     0.    1.     0.   |
  //  | -sin_chi  0.  cos_chi |
  CHI.set(
     cos_chi, 0., sin_chi,
        0.,   1.,    0.,
    -sin_chi, 0., cos_chi);

  // Matrix Phi
  //  |  cos_phi   sin_phi   0. |
  //  | -sin_phi   cos_phi   0. |
  //  |     0.        0.     1. |
  PHI.set(
     cos_phi, sin_phi, 0.,
    -sin_phi, cos_phi, 0.,
        0.,      0.,   1.);

  // Matrix Nu
  //  |  1.     0.       0. |
  //  |  0.  cos_nu  -sin_nu|
  //  |  0.  sin_nu   cos_nu|
  NU.set(
        1.,    0.,      0.,
        0., cos_nu, -sin_nu,
        0., sin_nu,  cos_nu);

  // Matrix Delta
  //  |  cos_delta   sin_delta 0. |
  //  | -sin_delta   cos_delta 0. |
  //  |     0.          0.     1. |
  DELTA.set(
     cos_delta, sin_delta, 0.,
    -sin_delta, cos_delta, 0.,
        0.,        0.,     1.);

  smatrix A(MU);
  A.multiplyOnTheRight(ETA);
  A.multiplyOnTheRight(CHI);
  A.multiplyOnTheRight(PHI);
  A.multiplyOnTheRight(UB);


  double det1 = A.get(1,1)*(A.get(2,2)*A.get(3,3)-A.get(3,2)*A.get(2,3));
  double det2 =-A.get(1,2)*(A.get(2,1)*A.get(3,3)-A.get(3,1)*A.get(2,3));
  double det3 = A.get(1,3)*(A.get(2,1)*A.get(3,2)-A.get(3,1)*A.get(2,2));
  double det = det1 + det2 + det3;

  if (fabs(det) < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "det(A) is null",
      "A = MU*ETA*CHI*PHI*U*B check if one of these matrices is null",
      "eulerian_horizontal4CBissectorMode6C::computeHKL()");

  // Solving Ax = b where in this case, b = k*(0., cos(nu)-1., sin(nu))
  double kk = physicalConstants::getTau() / lambda;
  double sum = 0.;
  double q1 = 0.;
  double q2 = kk*(cos(nu)-1.);
  double q3 = kk*sin(nu);

  sum = -q2 * (A.get(1,2)*A.get(3,3)-A.get(3,2)*A.get(1,3));
  sum = sum + q3 * (A.get(1,2)*A.get(2,3)-A.get(2,2)*A.get(1,3));
  h = sum / det;

  sum = q2 * (A.get(1,1)*A.get(3,3)-A.get(3,1)*A.get(1,3));
  sum = sum - q3 * (A.get(1,1)*A.get(2,3)-A.get(2,1)*A.get(1,3));
  k = sum / det;

  sum = -q2 * (A.get(1,1)*A.get(3,2)-A.get(3,1)*A.get(1,2));
  sum = sum + q3 * (A.get(1,1)*A.get(2,2)-A.get(2,1)*A.get(1,2));
  l = sum / det;
}

void eulerian_horizontal4CBissectorMode6C::printOnScreen() const
{}

//**********************************
//*** 4C BISECTOR VERTICAL MODE ***
//********************************
eulerian_vertical4CBissectorMode6C::eulerian_vertical4CBissectorMode6C()
{}

eulerian_vertical4CBissectorMode6C::~eulerian_vertical4CBissectorMode6C()
{}

// Solving equation (11) from :
// H. You "Angle calculations for a `4S+2D' six-circle diffractometer" (1999)
// Z11 * hphi1 + Z12 * hphi2 + Z13 * hphi3 = k*sin(delta)
// Z21 * hphi1 + Z22 * hphi2 + Z23 * hphi3 = k*(cos(delta)*cos(nu)-1.)
// Z31 * hphi1 + Z32 * hphi2 + Z33 * hphi3 = k*cos(delta)*sin(nu)
// where k = tau/lambda = q/2sin(theta) and
// mu = nu = 0.
//
// hphi1 = -q*sin(chi)*cos(phi)
// hphi2 = -q*sin(chi)*sin(phi))
// hphi3 =  q*cos(chi)
//
// If eta is constant :
// chi = arcsin(hphi3 / q)
// sin(phi) = hphi2 / (q*cos(chi))
// cos(phi) = hphi1 / (q*cos(chi))
angleConfiguration*
  eulerian_vertical4CBissectorMode6C::computeAngles(
  double h, double k, double l, const smatrix& UB, double lambda) const
  throw (HKLException)
{
  // h(theta) = R.hphi
  double mu;
  double nu;
  double eta;
  double chi;
  double phi;
  double theta;
  double delta;
  double sin_theta;
  double hphi_length;
  svector hphi(h,k,l);
  svector hphi_unitVector;
  eulerian_angleConfiguration6C* ac6C = 0;


  hphi.multiplyOnTheLeft(UB);
  hphi.unitVector(hphi_unitVector, hphi_length);

  if (fabs(lambda) < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "lamdba is null",
      "The wave length has not been set",
      "eulerian_vertical4CBissectorMode6C::computeAngles()");

  if ((fabs(h) < mathematicalConstants::getEpsilon1()) && 
      (fabs(k) < mathematicalConstants::getEpsilon1()) &&
      (fabs(l) < mathematicalConstants::getEpsilon1()))
  {
    throw HKLException(
      "(h,k,l) is null",
      "check your parameters",
      "eulerian_vertical4CBissectorMode6C::computeAngles()");
  }

  if (hphi.norminf() < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "hphi is null",
      "The matrix U has been computed from two parallel reflections or the crystal matrix is null",
      "eulerian_vertical4CBissectorMode6C::computeAngles()");

  /////////////////////
  // Bragg relation //
  ///////////////////
  // sin(theta) = || q || * lambda * 0.5 / tau.
  sin_theta = hphi_length * lambda * 0.5;
  // We have to be consistent with the conventions 
  // previously defined when we computed the crystal 
  // reciprocal lattice.
  sin_theta = sin_theta / physicalConstants::getTau();

  if (fabs(sin_theta) > 1.)
    throw HKLException(
      "sine bigger than 1.",
      "hphi_length too big, maybe error in UB matrix",
      "eulerian_vertical4CBissectorMode6C::computeAngles()");

  theta = asin(sin_theta);

  delta = 2*theta;

  // By definition in horizontal bisector mode.
  eta = 0.;
  mu = 0.;
  nu = 0.;

  double so = sin(eta);
  double co = cos(eta);

  double sx = hphi.get_Z() / (hphi_length * co);
  // SPEC : without hphi_length
  //double sx = hphi.get_Z() / (co);

  if (fabs(sx) > 1.)
    throw HKLException(
      "sine bigger than 1.",
      "hphi.getZ() too big or hphi_length too small, maybe error in UB matrix",
      "eulerian_vertical4CBissectorMode6C::computeAngles()");

  chi = asin(sx);
  // asin() returns values between -PI/2. and PI/2.
  // hphi 1st component sign tells whether chi belongs or not to
  // the other half of the circle i.e. between PI/2. and 3PI/2.
  if (hphi.get_X() < -mathematicalConstants::getEpsilon1())
    chi = 3.141592654 - chi;

  // We multiply by cos(chi) to get its sign in s_phi and c_phi knowing that
  // we 
  double c_phi = cos(chi)*hphi.get_X();
  double s_phi = cos(chi)*hphi.get_Y();

  if ((fabs(c_phi) < mathematicalConstants::getEpsilon1()) && 
      (fabs(s_phi) < mathematicalConstants::getEpsilon1()))
    phi = 0.;
  else
    phi = atan2(s_phi,c_phi);

  eta = theta;

  ac6C = new eulerian_angleConfiguration6C;
  ac6C->setDelta(delta);
  ac6C->setEta(eta);
  ac6C->setChi(chi);
  ac6C->setPhi(phi);
  ac6C->setNu(nu);
  ac6C->setMu(mu);

  return ac6C;
}

// Compute (h,k,l) from a sample of angles.
// Solve a linear system Ax = b where A is the product of the rotation matrices 
// MU, ETA, CHI, PHI by the orientation matrix U and the crystal matrix B. b is the
// scattering vector is : k(sin(delta), cos(delta).cos(nu)-1., cos(delta).sin(nu))
// and x = (h,k,l) and k = tau/lambda = q/2sin(theta). Raise an exception when det(A)=0.
void eulerian_vertical4CBissectorMode6C::computeHKL(
  double& h, double& k, double& l, const smatrix& UB, double lambda, angleConfiguration* ac) const
  throw (HKLException)
{
  double nu    = ((eulerian_angleConfiguration6C*)ac)->getNu();
  double mu    = ((eulerian_angleConfiguration6C*)ac)->getMu();
  double eta   = ((eulerian_angleConfiguration6C*)ac)->getEta();
  double chi   = ((eulerian_angleConfiguration6C*)ac)->getChi();
  double phi   = ((eulerian_angleConfiguration6C*)ac)->getPhi();
  double delta = ((eulerian_angleConfiguration6C*)ac)->getDelta();

  double cos_nu    = cos(nu);
  double sin_nu    = sin(nu);
  double cos_mu    = cos(mu);
  double sin_mu    = sin(mu);
  double cos_eta   = cos(eta);
  double sin_eta   = sin(eta);
  double cos_chi   = cos(chi);
  double sin_chi   = sin(chi);
  double cos_phi   = cos(phi);
  double sin_phi   = sin(phi);
  double cos_delta = cos(delta);
  double sin_delta = sin(delta);

  smatrix NU;
  smatrix MU;
  smatrix ETA;
  smatrix CHI;
  smatrix PHI;
  smatrix DELTA;

  // Matrix Mu
  //  |  1.     0.       0. |
  //  |  0.  cos_mu  -sin_mu|
  //  |  0.  sin_mu   cos_mu|
  MU.set(
        1.,    0.,      0.,
        0., cos_mu, -sin_mu,
        0., sin_mu,  cos_mu);

  // Matrix Eta
  //  |  cos_eta  sin_eta 0. |
  //  | -sin_eta  cos_eta 0. |
  //  |     0.       0.   1. |
  ETA.set(
     cos_eta, sin_eta, 0.,
    -sin_eta, cos_eta, 0.,
        0.,      0.,   1.);

  // Matrix Chi
  //  |  cos_chi  0.  sin_chi |
  //  |     0.    1.     0.   |
  //  | -sin_chi  0.  cos_chi |
  CHI.set(
     cos_chi, 0., sin_chi,
        0.,   1.,    0.,
    -sin_chi, 0., cos_chi);

  // Matrix Phi
  //  |  cos_phi   sin_phi   0. |
  //  | -sin_phi   cos_phi   0. |
  //  |     0.        0.     1. |
  PHI.set(
     cos_phi, sin_phi, 0.,
    -sin_phi, cos_phi, 0.,
        0.,      0.,   1.);

  // Matrix Nu
  //  |  1.     0.       0. |
  //  |  0.  cos_nu  -sin_nu|
  //  |  0.  sin_nu   cos_nu|
  NU.set(
        1.,    0.,      0.,
        0., cos_nu, -sin_nu,
        0., sin_nu,  cos_nu);

  // Matrix Delta
  //  |  cos_delta   sin_delta 0. |
  //  | -sin_delta   cos_delta 0. |
  //  |     0.          0.     1. |
  DELTA.set(
     cos_delta, sin_delta, 0.,
    -sin_delta, cos_delta, 0.,
        0.,        0.,     1.);

  smatrix A(MU);
  A.multiplyOnTheRight(ETA);
  A.multiplyOnTheRight(CHI);
  A.multiplyOnTheRight(PHI);
  A.multiplyOnTheRight(UB);


  double det1 = A.get(1,1)*(A.get(2,2)*A.get(3,3)-A.get(3,2)*A.get(2,3));
  double det2 =-A.get(1,2)*(A.get(2,1)*A.get(3,3)-A.get(3,1)*A.get(2,3));
  double det3 = A.get(1,3)*(A.get(2,1)*A.get(3,2)-A.get(3,1)*A.get(2,2));
  double det = det1 + det2 + det3;

  if (fabs(det) < mathematicalConstants::getEpsilon1())
    throw HKLException(
      "det(A) is null",
      "A = MU*ETA*CHI*PHI*U*B check if one of these matrices is null",
      "eulerian_vertical4CBissectorMode6C::computeHKL()");

  // Solving Ax = b where in this case, b = k*(0., cos(nu)-1., sin(nu))
  double kk = physicalConstants::getTau() / lambda;
  double sum = 0.;
  double q1 = kk*sin(delta);
  double q2 = kk*(cos(delta)-1.);
  double q3 = 0.;

  sum = -q2 * (A.get(1,2)*A.get(3,3)-A.get(3,2)*A.get(1,3));
  sum = sum + q1 * (A.get(2,2)*A.get(3,3)-A.get(3,2)*A.get(2,3));
  h = sum / det;

  sum = q2 * (A.get(1,1)*A.get(3,3)-A.get(3,1)*A.get(1,3));
  sum = sum - q1 * (A.get(2,1)*A.get(3,3)-A.get(3,1)*A.get(2,3));
  k = sum / det;

  sum = -q2 * (A.get(1,1)*A.get(3,2)-A.get(3,1)*A.get(1,2));
  sum = sum +q1 * (A.get(2,1)*A.get(3,2)-A.get(3,1)*A.get(2,2));
  l = sum / det;
}

void eulerian_vertical4CBissectorMode6C::printOnScreen() const
{}