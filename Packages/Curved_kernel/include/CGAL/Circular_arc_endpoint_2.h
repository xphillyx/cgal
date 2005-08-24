// Copyright (c) 2003  INRIA Sophia-Antipolis (France) and
//                     Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
//
// Authors : Monique Teillaud <Monique.Teillaud@sophia.inria.fr>
//           Sylvain Pion     <Sylvain.Pion@sophia.inria.fr>
// 
// Partially supported by the IST Programme of the EU as a Shared-cost
// RTD (FET Open) Project under Contract No  IST-2000-26473 
// (CGAL - Effective Computational Geometry for Curves and Surfaces) 

// file : include/CGAL/Circular_arc_endpoint_2.h

#ifndef CGAL_CIRCULAR_ARC_POINT_2_H
#define CGAL_CIRCULAR_ARC_POINT_2_H
namespace CGAL {

template < typename CurvedKernel >
class Circular_arc_point_2
  : public CurvedKernel::Kernel_base::Circular_arc_point_2
{
  typedef typename CurvedKernel::Kernel_base::Circular_arc_point_2 
                                           RCircular_arc_point_2;
  typedef typename CurvedKernel::Circle_2                  Circle_2;

  typedef typename CurvedKernel::Root_of_2               Root_of_2;

public:
  typedef typename CurvedKernel::Algebraic_kernel::Root_for_circles_2_2 
    Root_for_circles_2_2;
  typedef CurvedKernel   R; 
  typedef RCircular_arc_point_2 Rep;
  

 const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  Circular_arc_point_2()
    : RCircular_arc_point_2(
      typename R::Construct_circular_arc_endpoint_2()())
      {}


  Circular_arc_point_2(const Root_for_circles_2_2 & np)
    : RCircular_arc_point_2(
      typename R::Construct_circular_arc_endpoint_2()(np))
      {}

  Circular_arc_point_2(const RCircular_arc_point_2 & p)
    : RCircular_arc_point_2(p)
      {}
      
      
  const Root_of_2 & x() const
    { return typename R::Compute_x_2()(*this);}

  const Root_of_2 & y() const
    { return typename R::Compute_y_2()(*this);}

  Bbox_2  bbox() const
    { return typename R::Construct_bbox_2()(*this);}


};

template < typename CurvedKernel >
inline
bool
operator==(const Circular_arc_point_2<CurvedKernel> &p,
           const Circular_arc_point_2<CurvedKernel> &q)
{
  return CurvedKernel().equal_2_object()(p, q);
}

template < typename CurvedKernel >
inline
bool
operator!=(const Circular_arc_point_2<CurvedKernel> &p,
           const Circular_arc_point_2<CurvedKernel> &q)
{
  return ! (p == q);
}

} // namespace CGAL

#endif // CGAL_CIRCULAR_ARC_POINT_2_H
