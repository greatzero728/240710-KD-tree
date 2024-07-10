/**
 * @file CS350Loader.hpp
 * @author Eder Beldad (eder.b1@digipen.edu)
 * @brief Describe basic functionality for loading CS350 files
 * @date 2023-05-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CS350LOADER_HPP
#define CS350LOADER_HPP

#include "Math.hpp"

#include <vector>
#include <string>
#include <array>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "Math.hpp"
#include "Utils.hpp"
#include "Geometry.hpp"
#include "Logging.hpp"
#include "ShapeUtils.hpp"
namespace CS350 {

    /**
     * CS350_binary format description.
     *
     * 	- Filename example: "mirlo_0.cs350_binary".
     * 	- File starts with the signature "CS350". This is, the first 5 bytes of this file must be: ['C', 'S', '3', '5', '0']
     * 	- A vertex count follows (unsigned 4 bytes).
     * 	- An index count follows (unsigned 4 bytes), if 0, mesh is to be interpreted as a sequence of triangles.
     *	- An attribute follows describing if positions exist: (1 byte bool), if true, file will contain positions. Always set to true.
     *	- An attribute follows describing if normals exist: (1 byte bool), if true, file will contain normals.
     *	- An attribute follows describing if uvs exist: (1 byte bool), if true, file will contain uvs.
     *	- [vertex_count] vertices follow. For each vertex
     *		- A position will follow (if positions present): (3 floats)
     *		- A normal will follow (if normals present): (3 floats)
     *		- An uv will follow (if uvs present): (2 floats)
     *
     *	- Visual guide of file contents:
     *		["CS350"][vertexCount][indexCount][positionsPresent][normalsPresent][uvsPresent][POS1][NORMAL1][UV1]...[POSN][NORMALN][UVN]
     *		  ^          ^           ^           ^                 ^               ^           ^     ^        ^
     *		  Signature  4 bytes     4 bytes     1 byte            1 byte          1 byte   3floats 3floats  2floats
     *		   5 bytes
     *
     */

    /**
     * Describes a single simple primitive.
     * 	- It WILL have positions
     * 	- It may have uvs
     * 	- It may have normals
     * 	- If polygons:
     * 		- Is empty: Mesh is NOT indexed, every three {pos/[norm]/[uv]} will describe a triangle
     * 		- Is non-empty: Mesh is indexed, each face is described by a three index tuple
     */
    struct CS350PrimitiveData
    {
        using Face = std::array<int, 3>;
        std::vector<vec3> positions;
        std::vector<vec2> uvs;
        std::vector<vec3> normals;
        std::vector<Face> polygons;
        vec3 bvMin;
        vec3 bvMax;

        CS350PrimitiveData() : bvMin(vec3(0.0f)), bvMax(vec3(0.0f)) {}

    };


    
    /**
     * Describes a single scene object.
     *
     * Each object has:
     * 	- A primitive index (referring to a vector of primitives that has been previously loaded)
     * 	- A m2w, describing how that primitive should be represented
     */
    struct CS350SceneObject
    {
        int  primitiveIndex;
        mat4 m2w;
    };

    /**
     *
     */
    CS350PrimitiveData LoadCS350Binary(std::string const& file);

    /**
     *
     */
    std::vector<CS350SceneObject> LoadCS350Scene(std::string const& file);
}

#endif // CS350LOADER_HPP
