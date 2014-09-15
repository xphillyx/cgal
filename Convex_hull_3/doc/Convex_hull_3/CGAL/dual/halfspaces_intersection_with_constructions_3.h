namespace CGAL {

/*!
\ingroup PkgConvexHull3Functions

\brief computes the intersection of the halfspaces defined by the planes contained in the range [`begin`, `end`). The result is stored in the polyhedron `P`.
In order to do that, it is necessary to give the function a point inside the polyhedron named `origin` which is `CGAL::ORIGIN` by default.
This version constructs explicitly the dual points using the convex hull algorithm parametrized with the given traits class.

\attention Halfspaces are considered as lower halfspaces that is to say if the plane's equation is \f$ a\, x +b\, y +c\, z + d = 0 \f$ then the corresponding halfspace is defined by \f$ a\, x +b\, y +c\, z + d \le 0 \f$ .

\pre `origin` is inside the intersection of halfspaces defined by the range [`begin`, `end`).
\pre The computed intersection must be a bounded convex polyhedron.
\pre The value type of PlaneIterator (Plane) and the type of the origin (Point_3) must come from a CGAL kernel.

\tparam PlaneIterator must be an input iterator where the value type must be Polyhedron::Traits::Plane
\tparam Polyhedron must be a model of `ConvexHullPolyhedron_3`.
\tparam Traits must be a model of the concept `ConvexHullTraits_3`.

\sa `halfspaces_intersection_3()` 
 */

template <class PlaneIterator, class Polyhedron, class Traits>
void halfspaces_intersection_with_constructions_3(PlaneIterator pbegin,
                                                  PlaneIterator pend,
                                                  Polyhedron &P,
                                                  const Traits & ch_traits = Default_traits,
                                                  typename Polyhedron::Vertex::Point_3 const& origin = typename Polyhedron::Vertex::Point_3(CGAL::ORIGIN));

} /* namespace CGAL */
