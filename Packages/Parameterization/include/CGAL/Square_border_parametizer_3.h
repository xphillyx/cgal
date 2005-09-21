// Copyright (c) 2005  INRIA (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$
// $Name$
//
// Author(s)     : Laurent Saboret, Pierre Alliez


#ifndef CGAL_SQUAREBORDERPARAMETIZER_3_H
#define CGAL_SQUAREBORDERPARAMETIZER_3_H

#include <CGAL/parameterization_assertions.h>
#include <CGAL/Parametizer_traits_3.h>

#include <cfloat>
#include <climits>
#include <vector>

CGAL_BEGIN_NAMESPACE


//
// Class Square_border_parametizer_3
//

/// Base class of strategies that parameterize the border
/// of a 3D surface onto a square.
///
/// Implementation note:
/// To simplify the implementation, BorderParametizer_3 models know only the
/// MeshAdaptor_3 class. They don't know the parameterization algorithm
/// requirements nor the kind of sparse linear system used.
///
/// Concept: Model of the BorderParametizer_3 concept.
///
/// Design pattern:
/// BorderParametizer_3 models are Strategies (see [GOF95]): they implement
/// a strategy of boundary parameterization for models of MeshAdaptor_3

template<class MeshAdaptor_3>           //< 3D surface
class Square_border_parametizer_3
{
// Public types
public:
    // Export Mesh_Adaptor_3 type and subtypes
    typedef MeshAdaptor_3                   Adaptor;
    typedef typename Parametizer_traits_3<Adaptor>::Error_code
                                            Error_code;
    typedef typename Adaptor::NT            NT;
    typedef typename Adaptor::Facet_handle  Facet_handle;
    typedef typename Adaptor::Facet_const_handle
                                            Facet_const_handle;
    typedef typename Adaptor::Vertex_handle Vertex_handle;
    typedef typename Adaptor::Vertex_const_handle
                                            Vertex_const_handle;
    typedef typename Adaptor::Point_3       Point_3;
    typedef typename Adaptor::Point_2       Point_2;
    typedef typename Adaptor::Vector_3      Vector_3;
    typedef typename Adaptor::Vector_2      Vector_2;
    typedef typename Adaptor::Facet_iterator Facet_iterator;
    typedef typename Adaptor::Facet_const_iterator
                                            Facet_const_iterator;
    typedef typename Adaptor::Vertex_iterator Vertex_iterator;
    typedef typename Adaptor::Vertex_const_iterator
                                            Vertex_const_iterator;
    typedef typename Adaptor::Border_vertex_iterator
                                            Border_vertex_iterator;
    typedef typename Adaptor::Border_vertex_const_iterator
                                            Border_vertex_const_iterator;
    typedef typename Adaptor::Vertex_around_facet_circulator
                                            Vertex_around_facet_circulator;
    typedef typename Adaptor::Vertex_around_facet_const_circulator
                                            Vertex_around_facet_const_circulator;
    typedef typename Adaptor::Vertex_around_vertex_circulator
                                            Vertex_around_vertex_circulator;
    typedef typename Adaptor::Vertex_around_vertex_const_circulator
                                            Vertex_around_vertex_const_circulator;

// Public operations
public:
    /// Destructor of base class should be virtual
    virtual ~Square_border_parametizer_3() {}

    // Default constructor, copy constructor and operator =() are fine

    /// Assign to mesh's border vertices a 2D position (ie a (u,v) pair)
    /// on border's shape. Mark them as "parameterized".
    Error_code parameterize_border (Adaptor* mesh);

    /// Indicate if border's shape is convex
    bool  is_border_convex () { return true; }

// Protected operations
protected:
    /// compute length of an edge
    virtual double compute_edge_length(const Adaptor& mesh,
                                       Vertex_const_handle source,
                                       Vertex_const_handle target) = 0;

// Private types
private:
    typedef typename std::vector<double>    Offset_map;

// Private operations
private:
    /// compute total length of boundary
    double compute_boundary_length(const Adaptor& mesh);

    /// Compute mesh iterator whose offset is closest to 'value'
    Border_vertex_iterator closest_iterator(Adaptor* mesh,
                                            const Offset_map& offsets,
                                            double value);
};


/// compute  total length of boundary
template<class Adaptor>
inline
double Square_border_parametizer_3<Adaptor>::compute_boundary_length(
                                                        const Adaptor& mesh)
{
    double len = 0.0;
    for(Border_vertex_const_iterator it = mesh.mesh_main_border_vertices_begin();
        it != mesh.mesh_main_border_vertices_end();
        it++)
    {
        CGAL_parameterization_assertion(mesh.is_vertex_on_main_border(it));

        // Get next iterator (looping)
        Border_vertex_const_iterator next = it;
        next++;
        if(next == mesh.mesh_main_border_vertices_end())
            next = mesh.mesh_main_border_vertices_begin();

        // Add 'length' of it -> next vector to 'len'
        len += compute_edge_length(mesh, it, next);
    }
    return len;
}

/// Assign to mesh's border vertices a 2D position (ie a (u,v) pair)
/// on border's shape. Mark them as "parameterized".
template<class Adaptor>
inline
typename Parametizer_traits_3<Adaptor>::Error_code
Square_border_parametizer_3<Adaptor>::parameterize_border(Adaptor* mesh)
{
    CGAL_parameterization_assertion(mesh != NULL);

    // Nothing to do if no boundary
    if (mesh->mesh_main_border_vertices_begin() == mesh->mesh_main_border_vertices_end())
    {
        std::cerr << "  error ERROR_INVALID_BOUNDARY!" << std::endl;
        return Parametizer_traits_3<Adaptor>::ERROR_INVALID_BOUNDARY;
    }

    // compute the total boundary length
    double total_len = compute_boundary_length(*mesh);
    std::cerr << "  total boundary len: " << total_len << std::endl;
    if (total_len == 0)
    {
        std::cerr << "  error ERROR_INVALID_BOUNDARY!" << std::endl;
        return Parametizer_traits_3<Adaptor>::ERROR_INVALID_BOUNDARY;
    }

    // map to [0,4[
    std::cerr << "  map on a square...\n";
    double len = 0.0;           // current position on square in [0, total_len[
    Offset_map offset;          // vertex index -> offset map
    offset.reserve(mesh->count_mesh_vertices());
    Border_vertex_iterator it;
    for(it = mesh->mesh_main_border_vertices_begin();
        it != mesh->mesh_main_border_vertices_end();
        it++)
    {
        CGAL_parameterization_assertion(mesh->is_vertex_on_main_border(it));

        offset[mesh->get_vertex_index(it)] = 4.0f*len/total_len;
                                // current position on square in [0,4[

        // Get next iterator (looping)
        Border_vertex_iterator next = it;
        next++;
        if(next == mesh->mesh_main_border_vertices_end())
            next = mesh->mesh_main_border_vertices_begin();

        // Add edge "length" to 'len'
        len += compute_edge_length(*mesh, it, next);
    }

    // First square corner is mapped to first vertex.
    // Then find closest points for three other corners.
    Border_vertex_iterator it0 = mesh->mesh_main_border_vertices_begin();
    Border_vertex_iterator it1 = closest_iterator(mesh, offset, 1.0);
    Border_vertex_iterator it2 = closest_iterator(mesh, offset, 2.0);
    Border_vertex_iterator it3 = closest_iterator(mesh, offset, 3.0);
    //
    // We may get into trouble if the boundary is too short
    if (it0 == it1 || it1 == it2 || it2 == it3 || it3 == it0)
    {
        std::cerr << "  error ERROR_INVALID_BOUNDARY!" << std::endl;
        return Parametizer_traits_3<Adaptor>::ERROR_INVALID_BOUNDARY;
    }
    //
    // Snap these vertices to corners
    offset[mesh->get_vertex_index(it0)] = 0.0;
    offset[mesh->get_vertex_index(it1)] = 1.0;
    offset[mesh->get_vertex_index(it2)] = 2.0;
    offset[mesh->get_vertex_index(it3)] = 3.0;

    // Set vertices along square's sides and mark them as "parameterized"
    for(it = it0; it != it1; it++) // 1st side
    {
        Point_2 uv(offset[mesh->get_vertex_index(it)], 0.0);
        mesh->set_vertex_uv(it, uv);
        mesh->set_vertex_parameterized(it, true);
    }
    for(it = it1; it != it2; it++) // 2nd side
    {
        Point_2 uv(1.0, offset[mesh->get_vertex_index(it)]-1);
        mesh->set_vertex_uv(it, uv);
        mesh->set_vertex_parameterized(it, true);
    }
    for(it = it2; it != it3; it++) // 3rd side
    {
        Point_2 uv(3-offset[mesh->get_vertex_index(it)], 1.0);
        mesh->set_vertex_uv(it, uv);
        mesh->set_vertex_parameterized(it, true);
    }
    for(it = it3; it != mesh->mesh_main_border_vertices_end(); it++) // 4th side
    {
        Point_2 uv(0.0, 4-offset[mesh->get_vertex_index(it)]);
        mesh->set_vertex_uv(it, uv);
        mesh->set_vertex_parameterized(it, true);
    }

    std::cerr << "    done" << std::endl;

    return Parametizer_traits_3<Adaptor>::OK;
}

/// Utility method for parameterize_border()
/// Compute mesh iterator whose offset is closest to 'value'
template<class Adaptor>
inline
typename Adaptor::Border_vertex_iterator
Square_border_parametizer_3<Adaptor>::closest_iterator(Adaptor* mesh,
                                                       const Offset_map& offset,
                                                       double value)
{
    Border_vertex_iterator best;
    double min = DBL_MAX;           // distance for 'best'

    for (Border_vertex_iterator it = mesh->mesh_main_border_vertices_begin();
         it != mesh->mesh_main_border_vertices_end();
         it++)
    {
        double d = CGAL_CLIB_STD::fabs(offset[mesh->get_vertex_index(it)] - value);
        if (d < min)
        {
            best = it;
            min = d;
        }
    }

    return best;
}


//
// Class Square_border_uniform_parametizer_3
//

/// This class parameterizes the border of a 3D surface onto a square
/// on an uniform manner: points are equally spaced.
///
/// Concept: Model of the BorderParametizer_3 concept.

template<class MeshAdaptor_3>           //< 3D surface
class Square_border_uniform_parametizer_3
    : public Square_border_parametizer_3<MeshAdaptor_3>
{
// Public types
public:
    // Export Mesh_Adaptor_3 type and subtypes
    typedef MeshAdaptor_3                   Adaptor;
    typedef typename Parametizer_traits_3<Adaptor>::Error_code
                                            Error_code;
    typedef typename Adaptor::NT            NT;
    typedef typename Adaptor::Facet_handle  Facet_handle;
    typedef typename Adaptor::Facet_const_handle
                                            Facet_const_handle;
    typedef typename Adaptor::Vertex_handle Vertex_handle;
    typedef typename Adaptor::Vertex_const_handle
                                            Vertex_const_handle;
    typedef typename Adaptor::Point_3       Point_3;
    typedef typename Adaptor::Point_2       Point_2;
    typedef typename Adaptor::Vector_3      Vector_3;
    typedef typename Adaptor::Vector_2      Vector_2;
    typedef typename Adaptor::Facet_iterator Facet_iterator;
    typedef typename Adaptor::Facet_const_iterator
                                            Facet_const_iterator;
    typedef typename Adaptor::Vertex_iterator Vertex_iterator;
    typedef typename Adaptor::Vertex_const_iterator
                                            Vertex_const_iterator;
    typedef typename Adaptor::Border_vertex_iterator
                                            Border_vertex_iterator;
    typedef typename Adaptor::Border_vertex_const_iterator
                                            Border_vertex_const_iterator;
    typedef typename Adaptor::Vertex_around_facet_circulator
                                            Vertex_around_facet_circulator;
    typedef typename Adaptor::Vertex_around_facet_const_circulator
                                            Vertex_around_facet_const_circulator;
    typedef typename Adaptor::Vertex_around_vertex_circulator
                                            Vertex_around_vertex_circulator;
    typedef typename Adaptor::Vertex_around_vertex_const_circulator
                                            Vertex_around_vertex_const_circulator;

// Public operations
public:
    // Default constructor, copy constructor and operator =() are fine

// Protected operations
protected:
    /// compute length of an edge
    virtual double compute_edge_length(const Adaptor& mesh,
                                       Vertex_const_handle source,
                                       Vertex_const_handle target)
    {
        /// uniform boundary parameterization: points are equally spaced
        return 1;
    }
};


///
/// Class Square_border_arc_length_parametizer_3
///

/// This class parameterizes the border of a 3D surface onto a square,
/// with an arc-length parameterization: (u,v) values are
/// proportional to the length of boundary edges.
///
/// Concept: Model of the BorderParametizer_3 concept.

template<class MeshAdaptor_3>           //< 3D surface
class Square_border_arc_length_parametizer_3
    : public Square_border_parametizer_3<MeshAdaptor_3>
{
// Public types
public:
    // Export Mesh_Adaptor_3 type and subtypes
    typedef MeshAdaptor_3                   Adaptor;
    typedef typename Parametizer_traits_3<Adaptor>::Error_code
                                            Error_code;
    typedef typename Adaptor::NT            NT;
    typedef typename Adaptor::Facet_handle  Facet_handle;
    typedef typename Adaptor::Facet_const_handle
                                            Facet_const_handle;
    typedef typename Adaptor::Vertex_handle Vertex_handle;
    typedef typename Adaptor::Vertex_const_handle
                                            Vertex_const_handle;
    typedef typename Adaptor::Point_3       Point_3;
    typedef typename Adaptor::Point_2       Point_2;
    typedef typename Adaptor::Vector_3      Vector_3;
    typedef typename Adaptor::Vector_2      Vector_2;
    typedef typename Adaptor::Facet_iterator Facet_iterator;
    typedef typename Adaptor::Facet_const_iterator
                                            Facet_const_iterator;
    typedef typename Adaptor::Vertex_iterator Vertex_iterator;
    typedef typename Adaptor::Vertex_const_iterator
                                            Vertex_const_iterator;
    typedef typename Adaptor::Border_vertex_iterator
                                            Border_vertex_iterator;
    typedef typename Adaptor::Border_vertex_const_iterator
                                            Border_vertex_const_iterator;
    typedef typename Adaptor::Vertex_around_facet_circulator
                                            Vertex_around_facet_circulator;
    typedef typename Adaptor::Vertex_around_facet_const_circulator
                                            Vertex_around_facet_const_circulator;
    typedef typename Adaptor::Vertex_around_vertex_circulator
                                            Vertex_around_vertex_circulator;
    typedef typename Adaptor::Vertex_around_vertex_const_circulator
                                            Vertex_around_vertex_const_circulator;

// Public operations
public:
    // Default constructor, copy constructor and operator =() are fine

// Protected operations
protected:
    /// compute length of an edge
    virtual double compute_edge_length(const Adaptor& mesh,
                                       Vertex_const_handle source,
                                       Vertex_const_handle target)
    {
        /// arc-length boundary parameterization: (u,v) values are
        /// proportional to the length of boundary edges
        Vector_3 v = mesh.get_vertex_position(target)
                   - mesh.get_vertex_position(source);
        return std::sqrt(v*v);
    }
};


CGAL_END_NAMESPACE

#endif //CGAL_SQUAREBORDERPARAMETIZER_3_H

