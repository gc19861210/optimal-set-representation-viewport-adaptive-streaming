#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <cmath>

typedef  double SCALAR;

namespace IMT {
class Vector
{
public:
  Vector(void): m_x(0), m_y(0), m_z(0) {}
  Vector(SCALAR x, SCALAR y, SCALAR z): m_x(x), m_y(y), m_z(z) {}

  SCALAR DotProduct(const Vector& v) const {return m_x*v.m_x + m_y*v.m_y + m_z*v.m_z;}
  SCALAR Norm(void) const {return std::sqrt(DotProduct(*this));}

  Vector operator+(const Vector& v) const {return Vector(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z);}
  Vector operator-(void) const {return Vector(-m_x, -m_y, -m_z);}
  Vector operator-(const Vector& v) const {return Vector(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z);}
  Vector operator*(const SCALAR& s) const {return Vector(s*m_x, s*m_y, s*m_z);}
  Vector operator/(const SCALAR& s) const {return Vector(m_x/s, m_y/s, m_z/s);}
  //dot product
  SCALAR operator*(const Vector& v) const {return DotProduct(v);}
  //Vector product
  Vector operator^(const Vector& v) const {return Vector(m_y*v.m_z-m_z*v.m_y,
                    m_z*v.m_x-m_x*v.m_z,
                    m_x*v.m_y-m_y*v.m_x);}

  bool operator==(const Vector& v) const
  {
    return m_x == v.m_x && m_y == v.m_y &&  m_z == v.m_z;
  }
  bool operator!=(const Vector& v) const
  {
    return !(*this == v);
  }

  Vector VectorProduct(const Vector& v) const {return (*this)^v; }

  std::ostream& operator<<(std::ostream& o) const
  {
    o << "(" << m_x << ", "  << m_y << ", " <<  m_z << ")";
    return o;
  }

  auto ToSpherical(void) const
  {
    auto theta = std::atan2(m_y, m_x);
    auto phi = std::acos(m_z/Norm());
    return std::make_tuple(theta, phi);
  }

  static Vector FromSpherical(SCALAR theta, SCALAR phi)
  {
    auto sinP = std::sin(phi);
    return Vector(sinP*std::cos(theta), sinP*std::sin(theta), std::cos(phi));
  }

  auto GetX(void) const {return m_x;}
  auto GetY(void) const {return m_y;}
  auto GetZ(void) const {return m_z;}
private:
  SCALAR m_x;
  SCALAR m_y;
  SCALAR m_z;
};
inline std::ostream& operator<<(std::ostream& o, const Vector& v) {return v.operator<<(o);}
inline Vector operator*(const SCALAR& s, const Vector& v) {return v * s;}

struct not_unit_quaternion_exception : std::exception
{
  char const* what() const throw() { return "Rotation require unit quaternion"; }
};


class Quaternion
{
public:
  Quaternion(void): m_w(0), m_v(), m_isNormalized(false) {}
  Quaternion(const SCALAR& w, const Vector& v): m_w(w), m_v(v), m_isNormalized(false) {}
  Quaternion(const SCALAR& w): m_w(w), m_v(), m_isNormalized(false) {}
  Quaternion(const Vector& v): m_w(0), m_v(v), m_isNormalized(false) {}
  static Quaternion FromEuler(const SCALAR& yaw, const SCALAR& pitch, const SCALAR& roll)
  {
    double t0 = std::cos(yaw * 0.5);
  	double t1 = std::sin(yaw * 0.5);
  	double t2 = std::cos(roll * 0.5);
  	double t3 = std::sin(roll * 0.5);
  	double t4 = std::cos(pitch * 0.5);
  	double t5 = std::sin(pitch * 0.5);

    return Quaternion(t0 * t2 * t4 + t1 * t3 * t5, Vector(t0 * t3 * t4 - t1 * t2 * t5,
                                                          t0 * t2 * t5 + t1 * t3 * t4,
                                                          t1 * t2 * t4 - t0 * t3 * t5));
  }

  SCALAR DotProduct(const Quaternion& q) const {return m_w*q.m_w + m_v.DotProduct(q.m_v);}
  SCALAR Norm(void) const {return std::sqrt(DotProduct(*this));}

  Quaternion operator+(const Quaternion& q) const
  {
    return Quaternion(m_w+q.m_w, m_v+q.m_v);
  }
  Quaternion operator-(const Quaternion& q) const
  {
    return Quaternion(m_w-q.m_w, m_v-q.m_v);
  }
  Quaternion operator-(void) const
  {
    return Quaternion(-m_w, -m_v);
  }
  Quaternion operator*(const Quaternion& q) const
  {
    return Quaternion((m_w*q.m_w) - (m_v*q.m_v),
                      (m_w*q.m_v) + (q.m_w*m_v) + (m_v ^ q.m_v));
  }
  Quaternion operator*(SCALAR s) const
  {
    return Quaternion(m_w*s, m_v*s);
  }
  Quaternion operator/(const SCALAR& s) const
  {
    return Quaternion(m_w/s, m_v/s);
  }

  std::ostream& operator<<(std::ostream& o) const
  {
    o << m_w << " + "  << m_v.GetX() << " i + " << m_v.GetY() << " j + " << m_v.GetZ() << " k ";
    return o;
  }

  Quaternion& operator=(const Quaternion& q)
  {
    m_w = q.m_w;
    m_v = q.m_v;
    return *this;
  }

  bool operator==(const Quaternion& q) const
  {
    return m_w == q.m_w && m_v == q.m_v;
  }
  bool operator!=(const Quaternion& q) const
  {
    return !(*this == q);
  }

  void Normalize(void)
  {
    if (!m_isNormalized)
    {
      *this = *this / Norm();
      m_isNormalized = true;
    }
  }

  auto GetW(void) const {return m_w;}
  auto GetV(void) const {return m_v;}

  bool IsPur(void) const {return m_w == 0;}
  Quaternion Conj(void) const {return Quaternion(m_w, -m_v);}
  Quaternion Inv(void) const {return m_isNormalized ? Conj() : Conj()/std::pow(Norm(),2);}
  Vector Rotation(const Vector& v) const
  {
    if (m_isNormalized)
    {
      return ((*this)*v*(this->Conj())).GetV();
    }
    else
    {
      Quaternion q = *this;
      q.Normalize();
      return (q*v*q.Conj()).GetV();
    }
  }

  static Quaternion Exp(const Quaternion& q)
  {
    auto expW = std::exp(q.m_w);
    auto normV = q.m_v.Norm();
    return Quaternion(std::cos(normV)*expW,
                      normV != 0 ? std::sin(normV) * (q.m_v / normV) : q.m_v
                    );
  }
  static Quaternion Log(const Quaternion& q)
  {
    auto expW = std::log(q.Norm());
    auto normV = q.m_v.Norm();
    auto normQ = q.Norm();
    return Quaternion(std::log(normQ),
                      normV != 0 && normQ != 0 ? std::acos(q.m_w/normQ)*(q.m_v/normV) : q.m_v
                    );
  }
  static SCALAR Distance(const Quaternion& q1, const Quaternion& q2)
  {
    return (q2-q1).Norm();
  }

  static SCALAR OrthodromicDistance(const Quaternion& q1, const Quaternion& q2)
  {
    auto origine = Vector(1, 0, 0);
    Quaternion p1 = q1.Rotation(origine);
    Quaternion p2 = q2.Rotation(origine);
    auto p = p1 * p2;
    // p1 and p2 are pur so -p.m_w is the dot product and p.m_v is the vector product of p1 and p2
    return std::atan2(p.m_v.Norm(), -p.m_w);
  }

  static Quaternion pow(const Quaternion& q, const SCALAR& k)
  {
    return Quaternion::Exp(Quaternion::Log(q) * k);
  }

  static Quaternion SLERP(const Quaternion& q1, const Quaternion& q2, const SCALAR& k)
  {
    if (q1.DotProduct(q2) < 0)
    {
      return q1 * Quaternion::pow(q1.Inv() * (-q2), k);
    }
    else
    {
      return q1 * Quaternion::pow(q1.Inv() * q2, k);
    }
  }

  static  Quaternion QuaternionFromAngleAxis(const SCALAR& theta, const Vector& u)
  {
    return Quaternion(std::cos(theta/2), std::sin(theta/2)*(u/u.Norm()));
  }

  static Vector AverageAngularVelocity(Quaternion q1, Quaternion q2, const SCALAR& deltaT)
  {
    if (q1.DotProduct(q2) < 0)
    {
      q2 = -q2;
    }
    if (!q1.IsPur())
    {
      if (!q1.m_isNormalized)
      {
        q1.Normalize();
      }
      q1 = q1.Rotation(Vector(1, 0, 0));
    }
    if (!q2.IsPur())
    {
      if (!q2.m_isNormalized)
      {
        q2.Normalize();
      }
      q2 = q2.Rotation(Vector(1, 0, 0));
    }
    auto deltaQ = q2 - q1;
    auto W = (deltaQ * (2.0 / deltaT))*q1.Inv();
    return W.m_v;
  }

private:
  SCALAR m_w;
  Vector m_v;
  bool m_isNormalized;
};

inline std::ostream& operator<<(std::ostream& o, const Quaternion& q) {return q.operator<<(o);}
inline Quaternion operator+(const SCALAR& s, const Quaternion& q) {return Quaternion(s) + q;}
inline Quaternion operator-(const SCALAR& s, const Quaternion& q) {return Quaternion(s) - q;}
inline Quaternion operator+(const Vector& v, const Quaternion& q) {return Quaternion(v) + q;}
inline Quaternion operator-(const Vector& v, const Quaternion& q) {return Quaternion(v) - q;}
inline Quaternion operator+(const Vector& v, const SCALAR& s) {return Quaternion(v) + Quaternion(s);}
inline Quaternion operator-(const Vector& v, const SCALAR& s) {return Quaternion(v) - Quaternion(s);}
inline Quaternion operator*(const SCALAR& s, const Quaternion& q) {return q * s;}
inline Quaternion operator*(const Vector& v, const Quaternion& q) {return Quaternion(v) * q;}
inline Quaternion operator*(const Quaternion& q, const Vector& v) {return q * Quaternion(v);}

inline Quaternion pow(const Quaternion& q, const SCALAR& k)
{
  return Quaternion::pow(q, k);
}
}
