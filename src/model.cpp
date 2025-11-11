#include "model.h"
#include <fstream>
#include <sstream>

Model::Model(const std::string filename)
{
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail())
  {
    std::cerr << "Error, could not find the file" << std::endl;
    return;
  }

  std::string line;
  while (!in.eof())
  {
    std::getline(in, line);
    // std::istringstream << processes types properly. e.g. double, int, and
    // char.
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v "))
    {
      iss >> trash;
      vec3 v;

      double x, y, z = 0.0;

      // Check extraction success.
      if (iss >> x >> y >> z)
      {
        v.x = x;
        v.y = y;
        v.z = z;
        verts.push_back(v);
      }
      else
      {
        RNDR_ASSERT(!"Extraction of vector failed.");
        std::cerr << "Error: the obj file is supposed to have a correct vector."
                  << std::endl;
      }
    }
    else if (!line.compare(0, 2, "f "))
    {
      int face_idx, texture_idx, normal_idx, count = 0;

      iss >> trash;

      while (iss >> face_idx >> trash >> texture_idx >> trash >> normal_idx)
      {
        facet_vrt.push_back(--face_idx);
        count++;
      }

      if (count != 3)
      {
        RNDR_ASSERT(!"Extraction of face failed.");
        std::cerr << "Error: the obj file is supposed to be triangulated"
                  << std::endl;
        return;
      }
    }
  }
  std::cerr << "# v# " << nverts() << " f# " << nfaces() << std::endl;
}

int Model::nverts() const { return verts.size(); }
int Model::nfaces() const { return facet_vrt.size() / 3; }

vec3 Model::vert(const int i) const { return verts[i]; }

vec3 Model::vert(const int iface, const int nthvert) const
{
  return verts[facet_vrt[iface * 3 + nthvert]];
}
