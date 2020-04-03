#include <CGAL/Simple_cartesian.h>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh/IO/PLY.h>

#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/Linear_cell_complex_for_bgl_combinatorial_map_helper.h>
#include <CGAL/boost/graph/graph_traits_Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/boost/graph/helpers.h>

#include <CGAL/boost/graph/IO/PLY.h>

#if defined(CGAL_USE_OPENMESH)

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <CGAL/boost/graph/graph_traits_PolyMesh_ArrayKernelT.h>

#endif

#include <CGAL/boost/graph/io.h>

#include <iostream>
#include <fstream>

typedef CGAL::Simple_cartesian<double>                       Kernel;
typedef Kernel::Point_3                                      Point;

typedef CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3> Polyhedron;

typedef CGAL::Surface_mesh<Point>                            SM;

typedef CGAL::Linear_cell_complex_traits<3, Kernel> MyTraits;
typedef CGAL::Linear_cell_complex_for_bgl_combinatorial_map_helper
          <2, 3, MyTraits>::type LCC;

#if defined(CGAL_USE_OPENMESH)

typedef OpenMesh::PolyMesh_ArrayKernelT</* MyTraits*/> OMesh;

#endif

template<typename Mesh>
void test_bgl_read_write(const char* filename)
{
  Mesh sm;
  std::ifstream in(filename);
  CGAL::read_OFF(in,sm);
  CGAL::write_OFF(std::cout, sm);
}

#ifdef CGAL_USE_VTK
template<typename Mesh>
bool test_bgl_vtp()
{
  Mesh fg;
  CGAL::make_tetrahedron(Point(0, 0, 0), Point(1, 1, 0),
                         Point(2, 0, 1), Point(3, 0, 0), fg);

  std::ofstream os("tetrahedron.vtp");
  CGAL::write_VTP(os, fg);
  if(!os)
  {
    std::cerr<<"vtp writing failed."<<std::endl;
    return false;
  }
  os.close();
  Mesh fg2;
  if(!CGAL::read_VTP("tetrahedron.vtp", fg2))
  {
    std::cerr<<"vtp reading failed."<<std::endl;
    return false;
  }
  if(num_vertices(fg) != num_vertices(fg2)
     || num_faces(fg) != num_faces(fg2))
  {
    std::cerr<<"Coherence problem. Wrong number of vertices or faces."<<std::endl;
    std::cerr<<num_faces(fg)<<" != "<<num_faces(fg2)<<std::endl;
    return false;
  }
  return true;
}

template<>
bool test_bgl_vtp<Polyhedron>()
{
  Polyhedron fg;
  CGAL::make_tetrahedron(Point(0, 0, 0), Point(1, 1, 0),
                         Point(2, 0, 1), Point(3, 0, 0), fg);

  typedef boost::property_map<Polyhedron, CGAL::dynamic_vertex_property_t<std::size_t> >::type VertexIdMap;
   VertexIdMap vid  = get(CGAL::dynamic_vertex_property_t<std::size_t>(), fg);
   std::size_t id = 0;
   for(auto v : vertices(fg))
     put(vid,v, id++);
  std::ofstream os("tetrahedron.vtp");
  CGAL::write_VTP(os, fg, CGAL::parameters::vertex_index_map(vid));
  if(!os)
  {
    std::cerr<<"vtp writing failed."<<std::endl;
    return false;
  }
  os.close();
  Polyhedron fg2;
  if(!CGAL::read_VTP("tetrahedron.vtp", fg2))
  {
    std::cerr<<"vtp reading failed."<<std::endl;
    return false;
  }
  if(num_vertices(fg) != num_vertices(fg2)
     || num_faces(fg) != num_faces(fg2))
  {
    std::cerr<<"Coherence problem. Wrong number of vertices or faces."<<std::endl;
    return false;
  }
  return true;
}
#endif

template<class FaceGraph>
bool test_gocad()
{
  FaceGraph fg;
  CGAL::make_tetrahedron(Point(0, 0, 0), Point(1, 1, 0),
                         Point(2, 0, 1), Point(3, 0, 0), fg);
  std::ostringstream out;
  CGAL::write_GOCAD(out, "tetrahedron", fg);
  if(out.fail())
  {
    std::cerr<<"Tetrahedron writing failed."<<std::endl;
    return false;
  }
  FaceGraph fg2;
  std::istringstream in( out.str());
  std::string name, color;
  CGAL::read_GOCAD(in, name, color, fg2);
  if(name != "tetrahedron"){
    std::cerr<<"reading error: tetrahedron != "<<name<<std::endl;
    return 1;
  }
  if( !color.empty()){
    std::cerr<<"reading error: there should be no color."<<std::endl;
    return 1;
  }

  if(in.fail()){
    std::cerr<<"Tetrahedron reading failed."<<std::endl;
    return false;
  }

  if(num_vertices(fg2) != 4){
    std::cerr<<"Wrong number of vertices: 4 != "<<num_vertices(fg2)<<std::endl;
    return false;
  }

   if(num_faces(fg2) != 4)
  {
    std::cerr<<"Wrong number of faces: 4 != "<<num_faces(fg2)<<std::endl;
    return false;
  }

  return true;
}

template<class FaceGraph>
bool test_STL()
{
  FaceGraph fg;
  CGAL::make_tetrahedron(Point(0, 0, 0), Point(1, 1, 0),
                         Point(2, 0, 1), Point(3, 0, 0), fg);
  std::ostringstream out;
  CGAL::write_STL(out, fg);
  if(out.fail())
  {
    std::cerr<<"Tetrahedron writing failed."<<std::endl;
    return false;
  }
  FaceGraph fg2;
  std::istringstream in(out.str());
  if(!CGAL::read_STL(in, fg2)){
    std::cerr<<"Tetrahedron reading failed."<<std::endl;
    return false;
  }

  if(num_vertices(fg2) != 4){
    std::cerr<<"Wrong number of vertices: 4 != "<<num_vertices(fg2)<<std::endl;
    return false;
  }

   if(num_faces(fg2) != 4)
  {
    std::cerr<<"Wrong number of faces: 4 != "<<num_faces(fg2)<<std::endl;
    return false;
  }

  return true;
}


template<class FaceGraph>
bool test_PLY()
{
  FaceGraph fg;
  CGAL::make_tetrahedron(Point(0, 0, 0), Point(1, 1, 0),
                         Point(2, 0, 1), Point(3, 0, 0), fg);
  std::ostringstream out;
  CGAL::write_PLY(out, fg, "hello");
  if(out.fail())
  {
    std::cerr<<"Tetrahedron writing failed."<<std::endl;
    return false;
  }
  std::istringstream in(out.str());
  std::vector<Point> points;
  std::vector<std::vector<std::size_t> > polygons;
  fg.clear();
  if(!CGAL::read_PLY(in,fg, CGAL::parameters::all_default())){
    std::cerr<<"Tetrahedron reading failed."<<std::endl;
    return false;
  }

  CGAL_assertion(num_vertices(fg) == 4);
  CGAL_assertion(num_faces(fg) == 4);

  return true;
}


int main(int argc, char** argv)
{
  const char* filename=(argc>1) ? argv[1] : "data/prim.off";

  //PLY
  test_PLY<Polyhedron>();
  test_PLY<SM>();
  // OFF
  test_bgl_read_write<Polyhedron>(filename);
  test_bgl_read_write<SM>(filename);
  test_bgl_read_write<LCC>(filename);
#ifdef CGAL_USE_OPENMESH
  test_bgl_read_write<OMesh>(filename);
#endif

  // GOCAD
  if(!test_gocad<Polyhedron>())
    return 1;
  if(!test_gocad<SM>())
    return 1;
  if(!test_gocad<LCC>())
    return 1;

  // STL
  if(!test_STL<Polyhedron>())
    return 1;
  if(!test_STL<SM>())
    return 1;
  if(!test_STL<LCC>())
    return 1;

  // VTP
#ifdef CGAL_USE_VTK
  if(!test_bgl_vtp<Polyhedron>())
    return 1;
  if(!test_bgl_vtp<SM>())
    return 1;
  if(!test_bgl_vtp<LCC>())
    return 1;
#endif

  return EXIT_SUCCESS;
}
