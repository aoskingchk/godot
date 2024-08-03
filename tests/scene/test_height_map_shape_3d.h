/**************************************************************************/
/*  test_image_texture_3d.h                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef TEST_HEIGHT_MAP_SHAPE_3D_H
#define TEST_HEIGHT_MAP_SHAPE_3D_H

#include "scene/resources/3d/height_map_shape_3d.h"
#include "core/io/image.h"

#include "tests/test_macros.h"
#include "tests/test_utils.h"

namespace TestHeightMapShape3D {

TEST_CASE("[SceneTree][HeightMapShape3D] Constructor") {
    MESSAGE("Starting Constructor test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    MESSAGE("HeightMapShape3D object created");
    CHECK(height_map_shape->get_map_width() == 2);
    MESSAGE("Checked map width: ", height_map_shape->get_map_width());
    CHECK(height_map_shape->get_map_depth() == 2);
    MESSAGE("Checked map depth: ", height_map_shape->get_map_depth());
    CHECK(height_map_shape->get_map_data().size() == 4);
    MESSAGE("Checked map data size: ", height_map_shape->get_map_data().size());
    CHECK(height_map_shape->get_min_height() == 0.0);
    MESSAGE("Checked min height: ", height_map_shape->get_min_height());
    CHECK(height_map_shape->get_max_height() == 0.0);
    MESSAGE("Checked max height: ", height_map_shape->get_max_height());
}

TEST_CASE("[SceneTree][HeightMapShape3D] set_map_width and get_map_width") {
    MESSAGE("Starting set_map_width and get_map_width test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    height_map_shape->set_map_width(10);
    MESSAGE("Set map width to 10");
    CHECK(height_map_shape->get_map_width() == 10);
    MESSAGE("Checked map width: ", height_map_shape->get_map_width());
}

TEST_CASE("[SceneTree][HeightMapShape3D] set_map_depth and get_map_depth") {
    MESSAGE("Starting set_map_depth and get_map_depth test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    height_map_shape->set_map_depth(15);
    MESSAGE("Set map depth to 15");
    CHECK(height_map_shape->get_map_depth() == 15);
    MESSAGE("Checked map depth: ", height_map_shape->get_map_depth());
}

TEST_CASE("[SceneTree][HeightMapShape3D] set_map_data and get_map_data") {
    MESSAGE("Starting set_map_data and get_map_data test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    Vector<real_t> map_data;
    map_data.push_back(1.0);
    map_data.push_back(2.0);
    height_map_shape->set_map_data(map_data);
    MESSAGE("Set map data to [1.0, 2.0]");
    CHECK(height_map_shape->get_map_data().size() == 4.0);
    MESSAGE("Checked map data size: ", height_map_shape->get_map_data().size());
    CHECK(height_map_shape->get_map_data()[0] == 0.0);
    MESSAGE("Checked map data[0]: ", height_map_shape->get_map_data()[0]);
    CHECK(height_map_shape->get_map_data()[1] == 0.0);
    MESSAGE("Checked map data[1]: ", height_map_shape->get_map_data()[1]);
}

TEST_CASE("[SceneTree][HeightMapShape3D] get_min_height") {
    MESSAGE("Starting get_min_height test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    height_map_shape->set_map_data(Vector<real_t>{1.0, 2.0, 0.5});
    MESSAGE("Set map data to [1.0, 2.0, 0.5]");
    CHECK(height_map_shape->get_min_height() == 0.0);
    MESSAGE("Checked min height: ", height_map_shape->get_min_height());
}

TEST_CASE("[SceneTree][HeightMapShape3D] get_max_height") {
    MESSAGE("Starting get_max_height test case");
    Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
    height_map_shape->set_map_data(Vector<real_t>{1.0, 2.0, 0.5});
    MESSAGE("Set map data to [1.0, 2.0, 0.5]");
    CHECK(height_map_shape->get_max_height() == 0.0);
    MESSAGE("Checked max height: ", height_map_shape->get_max_height());
}

// TEST_CASE("[HeightMapShape3D] update_map_data_from_image") {
//     Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
// }

// TEST_CASE("[HeightMapShape3D] get_debug_mesh_lines") {
//     Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
// }

// TEST_CASE("[HeightMapShape3D] get_enclosing_radius") {
//     Ref<HeightMapShape3D> height_map_shape = memnew(HeightMapShape3D);
// }

} // namespace TestHeightMapShape3D

#endif // TEST_HEIGHT_MAP_SHAPE_3D_H
