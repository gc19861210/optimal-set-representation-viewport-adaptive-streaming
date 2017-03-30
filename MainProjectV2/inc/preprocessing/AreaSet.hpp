#pragma once
/*Class that regroup a set of area that partition the sphere*/

#include <vector>

#include "common/Common.hpp"

namespace IMT {
class Area
{
public:
  Area(double theta, double phi, double surface):
    m_theta(theta), m_phi(phi), m_surface(surface)
    {}
  bool Intersection(const RotMat& rotMat, double horizontalFoVAngle, double verticalFoVAngle) const
    {
      auto y = std::sqrt(1-std::cos(horizontalFoVAngle));
      auto z = std::sqrt(1-std::cos(verticalFoVAngle));
      auto a = Coord3dCart(1, y, z);
      auto b = Coord3dCart(1, y, -z);
      auto c = Coord3dCart(1, -y, -z);
      auto d = Coord3dCart(1, -y, z);
      // compute inward normal to the delimitation plan
      auto n_ab = a ^ b;
      n_ab = n_ab/norm(n_ab);
      auto n_bc = b ^ c;
      n_bc = n_bc/norm(n_bc);
      auto n_cd = c ^ d;
      n_cd = n_cd/norm(n_cd);
      auto n_da = d ^ a;
      n_da = n_da/norm(n_da);
      Coord3dCart refPixelPos = Rotation(Coord3dSpherical(1, m_theta, m_phi), rotMat.inv());
      return refPixelPos * n_ab >= 0 && refPixelPos * n_bc >= 0 &&
             refPixelPos * n_cd >= 0 && refPixelPos * n_da >= 0;
    }

    const auto& GetSurface(void) const {return m_surface;}
    const auto& GetTheta(void) const {return m_theta;}
    const auto& GetPhi(void) const {return m_phi;}
private:
  double m_theta;
  double m_phi;
  double m_surface;
};

class AreaSet
{
public:
  AreaSet(unsigned nbHPixels, unsigned nbVPixels): m_areas()
  {
    // double minArea = 18;
    // double maxArea = 0;
    for (unsigned j = 1; j < 2*nbVPixels; j += 2)
    {
      unsigned localNbH = std::ceil(nbHPixels*std::sin(j*PI/(2*nbVPixels)));
      for (unsigned i = 0; i < localNbH; ++i)
      {
        m_areas.push_back(Area(i*2*PI/localNbH - PI, j*PI/(2*nbVPixels), 2*PI*PI*sin(j*PI/(2*nbVPixels))/(localNbH*nbVPixels)));
        m_generatedAsQERCounter.push_back(0);
        // minArea = std::min(minArea, double(2*PI*PI*sin(j*PI/(2*nbVPixels))/(localNbH*nbVPixels)));
        // maxArea = std::max(maxArea, double(2*PI*PI*sin(j*PI/(2*nbVPixels))/(localNbH*nbVPixels)));
      }
    }
    // std::cout << "Min area = " << minArea << "Max area = " << maxArea << std::endl;
  }

  std::vector<bool> GetVisibility(const RotMat& rotMat, double horizontalFoVAngle, double verticalFoVAngle) const
  {
    std::vector<bool> ans;
    for (auto const& area: m_areas)
    {
      ans.push_back(area.Intersection(rotMat, horizontalFoVAngle, verticalFoVAngle));
    }
    return std::move(ans);
  }

  auto const& GetAreas(void) const {return m_areas;}

  std::vector<unsigned> GetAreaIdInTile(double startTheta, double endTheta, double startPhi, double endPhi)
  {
    std::vector<unsigned> ans;
    for (unsigned i = 0; i < m_areas.size(); ++i)
    {
      if (m_areas[i].GetTheta() >= startTheta && m_areas[i].GetTheta() < endTheta &&
          m_areas[i].GetPhi() >= startPhi && m_areas[i].GetPhi() < endPhi)
      {
        ans.push_back(i);
      }
    }
    return std::move(ans);
  }

  void AddUseAsQer(unsigned AreaId) {++m_generatedAsQERCounter[AreaId];}
private:
  std::vector<Area> m_areas;
  std::vector<unsigned> m_generatedAsQERCounter;
};
}
