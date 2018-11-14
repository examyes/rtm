////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2018 Nicholas Frechette & Animation Compression Library contributors
// Copyright (c) 2018 Nicholas Frechette & Realtime Math contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <catch.hpp>

#include <rtm/matrix3x4f.h>
#include <rtm/matrix3x4d.h>
#include <rtm/qvvf.h>
#include <rtm/qvvd.h>

using namespace rtm;

template<typename MatrixType, typename TransformType, typename FloatType>
static void test_affine_matrix_impl(const MatrixType& identity, const FloatType threshold)
{
	using QuatType = decltype(TransformType::rotation);
	using Vector4Type = decltype(TransformType::translation);

	{
		Vector4Type x_axis = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(0.0));
		Vector4Type y_axis = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0), FloatType(0.0));
		Vector4Type z_axis = vector_set(FloatType(7.0), FloatType(8.0), FloatType(9.0), FloatType(0.0));
		Vector4Type w_axis = vector_set(FloatType(10.0), FloatType(11.0), FloatType(12.0), FloatType(1.0));
		MatrixType mtx = matrix_set(x_axis, y_axis, z_axis, w_axis);
		REQUIRE(vector_all_near_equal(x_axis, mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(y_axis, mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(z_axis, mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(w_axis, mtx.w_axis, threshold));
	}

	{
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), identity.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), identity.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), identity.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0), FloatType(1.0)), identity.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		Vector4Type translation = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0));
		MatrixType mtx = matrix_set(rotation_around_z, translation, vector_set(FloatType(1.0)));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(-1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(1.0)), mtx.w_axis, threshold));

		Vector4Type scale = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0));
		mtx = matrix_set(rotation_around_z, translation, scale);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(4.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(-5.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(6.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(1.0)), mtx.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		MatrixType mtx = matrix_from_quat(rotation_around_z);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(-1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0), FloatType(1.0)), mtx.w_axis, threshold));
	}

	{
		MatrixType mtx = matrix_from_translation(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0)));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(1.0)), mtx.w_axis, threshold));
	}

	{
		MatrixType mtx = matrix_from_scale(vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0)));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(4.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(5.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(6.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0), FloatType(1.0)), mtx.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		Vector4Type translation = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0));
		Vector4Type scale = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0));
		TransformType transform = qvv_set(rotation_around_z, translation, scale);
		MatrixType mtx = matrix_from_qvv(transform);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(4.0), FloatType(0.0), FloatType(0.0)), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(-5.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(6.0), FloatType(0.0)), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(1.0)), mtx.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		Vector4Type translation = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0));
		Vector4Type scale = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0));
		MatrixType mtx = matrix_set(rotation_around_z, translation, scale);
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx, axis::x), mtx.x_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx, axis::y), mtx.y_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx, axis::z), mtx.z_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx, axis::w), mtx.w_axis, threshold));

		const MatrixType mtx2 = mtx;
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx2, axis::x), mtx2.x_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx2, axis::y), mtx2.y_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx2, axis::z), mtx2.z_axis, threshold));
		REQUIRE(vector_all_near_equal(matrix_get_axis(mtx2, axis::w), mtx2.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		MatrixType mtx = matrix_from_quat(rotation_around_z);
		QuatType rotation = quat_from_matrix(mtx);
		REQUIRE(quat_near_equal(rotation_around_z, rotation, threshold));
	}

	{
		Vector4Type x_axis = vector_set(FloatType(1.0), FloatType(0.0), FloatType(0.0));
		Vector4Type y_axis = vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0));

		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		MatrixType mtx_a = matrix_set(rotation_around_z, x_axis, vector_set(FloatType(1.0)));
		Vector4Type result = matrix_mul_position(mtx_a, x_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(1.0), FloatType(1.0), FloatType(0.0)), threshold));
		result = matrix_mul_position(mtx_a, y_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0)), threshold));

		QuatType rotation_around_x = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)));
		MatrixType mtx_b = matrix_set(rotation_around_x, y_axis, vector_set(FloatType(1.0)));
		result = matrix_mul_position(mtx_b, x_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(1.0), FloatType(1.0), FloatType(0.0)), threshold));
		result = matrix_mul_position(mtx_b, y_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(0.0), FloatType(1.0), FloatType(-1.0)), threshold));

		MatrixType mtx_ab = matrix_mul(mtx_a, mtx_b);
		MatrixType mtx_ba = matrix_mul(mtx_b, mtx_a);
		result = matrix_mul_position(mtx_ab, x_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(1.0), FloatType(1.0), FloatType(-1.0)), threshold));
		REQUIRE(vector_all_near_equal3(result, matrix_mul_position(mtx_b, matrix_mul_position(mtx_a, x_axis)), threshold));
		result = matrix_mul_position(mtx_ab, y_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0)), threshold));
		REQUIRE(vector_all_near_equal3(result, matrix_mul_position(mtx_b, matrix_mul_position(mtx_a, y_axis)), threshold));
		result = matrix_mul_position(mtx_ba, x_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0)), threshold));
		REQUIRE(vector_all_near_equal3(result, matrix_mul_position(mtx_a, matrix_mul_position(mtx_b, x_axis)), threshold));
		result = matrix_mul_position(mtx_ba, y_axis);
		REQUIRE(vector_all_near_equal3(result, vector_set(FloatType(0.0), FloatType(0.0), FloatType(-1.0)), threshold));
		REQUIRE(vector_all_near_equal3(result, matrix_mul_position(mtx_a, matrix_mul_position(mtx_b, y_axis)), threshold));
	}

	{
		Vector4Type x_axis = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(0.0));
		Vector4Type y_axis = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0), FloatType(0.0));
		Vector4Type z_axis = vector_set(FloatType(7.0), FloatType(8.0), FloatType(9.0), FloatType(0.0));
		Vector4Type w_axis = vector_set(FloatType(10.0), FloatType(11.0), FloatType(12.0), FloatType(1.0));
		MatrixType mtx0 = matrix_set(x_axis, y_axis, z_axis, w_axis);
		MatrixType mtx1 = rtm_impl::matrix_transpose(mtx0);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(4.0), FloatType(7.0), FloatType(10.0)), mtx1.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(2.0), FloatType(5.0), FloatType(8.0), FloatType(11.0)), mtx1.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(3.0), FloatType(6.0), FloatType(9.0), FloatType(12.0)), mtx1.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0), FloatType(1.0)), mtx1.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		Vector4Type translation = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0));
		Vector4Type scale = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0));
		MatrixType mtx = matrix_set(rotation_around_z, translation, scale);
		MatrixType inv_mtx = matrix_inverse(mtx);
		MatrixType result = matrix_mul(mtx, inv_mtx);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), result.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), result.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), result.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(0.0), FloatType(1.0)), result.w_axis, threshold));
	}

	{
		QuatType rotation_around_z = quat_from_euler(scalar_deg_to_rad(FloatType(0.0)), scalar_deg_to_rad(FloatType(90.0)), scalar_deg_to_rad(FloatType(0.0)));
		Vector4Type translation = vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0));
		Vector4Type scale = vector_set(FloatType(4.0), FloatType(5.0), FloatType(6.0));
		MatrixType mtx0 = matrix_set(rotation_around_z, translation, scale);
		MatrixType mtx0_no_scale = matrix_remove_scale(mtx0);
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(1.0), FloatType(0.0), FloatType(0.0)), mtx0_no_scale.x_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(-1.0), FloatType(0.0), FloatType(0.0), FloatType(0.0)), mtx0_no_scale.y_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(0.0), FloatType(0.0), FloatType(1.0), FloatType(0.0)), mtx0_no_scale.z_axis, threshold));
		REQUIRE(vector_all_near_equal(vector_set(FloatType(1.0), FloatType(2.0), FloatType(3.0), FloatType(1.0)), mtx0_no_scale.w_axis, threshold));
	}
}

TEST_CASE("matrix3x4f math", "[math][matrix3x4]")
{
	test_affine_matrix_impl<matrix3x4f, qvvf, float>(matrix_identity_32(), 1.0e-4f);

	{
		quatf rotation_around_z = quat_from_euler(scalar_deg_to_rad(0.0f), scalar_deg_to_rad(90.0f), scalar_deg_to_rad(0.0f));
		vector4f translation = vector_set(1.0f, 2.0f, 3.0f);
		vector4f scale = vector_set(4.0f, 5.0f, 6.0f);
		matrix3x4f src = matrix_set(rotation_around_z, translation, scale);
		matrix3x4d dst = matrix_cast(src);
		REQUIRE(vector_all_near_equal(vector_cast(src.x_axis), dst.x_axis, 1.0e-4));
		REQUIRE(vector_all_near_equal(vector_cast(src.y_axis), dst.y_axis, 1.0e-4));
		REQUIRE(vector_all_near_equal(vector_cast(src.z_axis), dst.z_axis, 1.0e-4));
		REQUIRE(vector_all_near_equal(vector_cast(src.w_axis), dst.w_axis, 1.0e-4));
	}
}

TEST_CASE("matrix3x4d math", "[math][matrix3x4]")
{
	test_affine_matrix_impl<matrix3x4d, qvvd, double>(matrix_identity_64(), 1.0e-4);

	{
		quatd rotation_around_z = quat_from_euler(scalar_deg_to_rad(0.0), scalar_deg_to_rad(90.0), scalar_deg_to_rad(0.0));
		vector4d translation = vector_set(1.0, 2.0, 3.0);
		vector4d scale = vector_set(4.0, 5.0, 6.0);
		matrix3x4d src = matrix_set(rotation_around_z, translation, scale);
		matrix3x4f dst = matrix_cast(src);
		REQUIRE(vector_all_near_equal(vector_cast(src.x_axis), dst.x_axis, 1.0e-4f));
		REQUIRE(vector_all_near_equal(vector_cast(src.y_axis), dst.y_axis, 1.0e-4f));
		REQUIRE(vector_all_near_equal(vector_cast(src.z_axis), dst.z_axis, 1.0e-4f));
		REQUIRE(vector_all_near_equal(vector_cast(src.w_axis), dst.w_axis, 1.0e-4f));
	}
}