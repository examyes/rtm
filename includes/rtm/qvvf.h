#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2017 Nicholas Frechette & Animation Compression Library contributors
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

#include "rtm/math.h"
#include "rtm/quatf.h"
#include "rtm/vector4f.h"
#include "rtm/matrix3x4f.h"
#include "rtm/impl/compiler_utils.h"
#include "rtm/impl/qvv_common.h"

RTM_IMPL_FILE_PRAGMA_PUSH

namespace rtm
{
	//////////////////////////////////////////////////////////////////////////
	// Casts a QVV transform float64 variant to a float32 variant.
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_cast(const qvvd& input) RTM_NO_EXCEPT
	{
		return qvvf{ quat_cast(input.rotation), vector_cast(input.translation), vector_cast(input.scale) };
	}

	//////////////////////////////////////////////////////////////////////////
	// Multiplies two QVV transforms.
	// Multiplication order is as follow: local_to_world = qvv_mul(local_to_object, object_to_world)
	// NOTE: When scale is present, multiplication will not properly handle skew/shear,
	// use affine matrices if you have issues.
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_mul(qvvf_arg0 lhs, qvvf_arg1 rhs) RTM_NO_EXCEPT
	{
		const vector4f min_scale = vector_min(lhs.scale, rhs.scale);
		const vector4f scale = vector_mul(lhs.scale, rhs.scale);

		if (vector_any_less_than3(min_scale, vector_zero()))
		{
			// If we have negative scale, we go through a matrix
			const matrix3x4f lhs_mtx = matrix_from_qvv(lhs);
			const matrix3x4f rhs_mtx = matrix_from_qvv(rhs);
			matrix3x4f result_mtx = matrix_mul(lhs_mtx, rhs_mtx);
			result_mtx = matrix_remove_scale(result_mtx);

			const vector4f sign = vector_sign(scale);
			result_mtx.x_axis = vector_mul(result_mtx.x_axis, vector_dup_x(sign));
			result_mtx.y_axis = vector_mul(result_mtx.y_axis, vector_dup_y(sign));
			result_mtx.z_axis = vector_mul(result_mtx.z_axis, vector_dup_z(sign));

			const quatf rotation = quat_from_matrix(result_mtx);
			const vector4f translation = result_mtx.w_axis;
			return qvv_set(rotation, translation, scale);
		}
		else
		{
			const quatf rotation = quat_mul(lhs.rotation, rhs.rotation);
			const vector4f translation = vector_add(quat_mul_vector3(vector_mul(lhs.translation, rhs.scale), rhs.rotation), rhs.translation);
			return qvv_set(rotation, translation, scale);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Multiplies two QVV transforms ignoring 3D scale.
	// The resulting QVV transform with have a [1,1,1] 3D scale.
	// Multiplication order is as follow: local_to_world = qvv_mul(local_to_object, object_to_world)
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_mul_no_scale(qvvf_arg0 lhs, qvvf_arg1 rhs) RTM_NO_EXCEPT
	{
		const quatf rotation = quat_mul(lhs.rotation, rhs.rotation);
		const vector4f translation = vector_add(quat_mul_vector3(lhs.translation, rhs.rotation), rhs.translation);
		return qvv_set(rotation, translation, vector_set(1.0f));
	}

	//////////////////////////////////////////////////////////////////////////
	// Multiplies a QVV transform and a 3D point.
	// Multiplication order is as follow: world_position = qvv_mul_point3(local_position, local_to_world)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL qvv_mul_point3(vector4f_arg0 point, qvvf_arg1 qvv) RTM_NO_EXCEPT
	{
		return vector_add(quat_mul_vector3(vector_mul(qvv.scale, point), qvv.rotation), qvv.translation);
	}

	//////////////////////////////////////////////////////////////////////////
	// Multiplies a QVV transform and a 3D point ignoring 3D scale.
	// Multiplication order is as follow: world_position = qvv_mul_point3_no_scale(local_position, local_to_world)
	//////////////////////////////////////////////////////////////////////////
	inline vector4f RTM_SIMD_CALL qvv_mul_point3_no_scale(vector4f_arg0 point, qvvf_arg1 qvv) RTM_NO_EXCEPT
	{
		return vector_add(quat_mul_vector3(point, qvv.rotation), qvv.translation);
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the inverse of the input QVV transform.
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_inverse(qvvf_arg0 input) RTM_NO_EXCEPT
	{
		const quatf inv_rotation = quat_conjugate(input.rotation);
		const vector4f inv_scale = vector_reciprocal(input.scale);
		const vector4f inv_translation = vector_neg(quat_mul_vector3(vector_mul(input.translation, inv_scale), inv_rotation));
		return qvv_set(inv_rotation, inv_translation, inv_scale);
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns the inverse of the input QVV transform ignoring 3D scale.
	// The resulting QVV transform with have a [1,1,1] 3D scale.
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_inverse_no_scale(qvvf_arg0 input) RTM_NO_EXCEPT
	{
		const quatf inv_rotation = quat_conjugate(input.rotation);
		const vector4f inv_translation = vector_neg(quat_mul_vector3(input.translation, inv_rotation));
		return qvv_set(inv_rotation, inv_translation, vector_set(1.0f));
	}

	//////////////////////////////////////////////////////////////////////////
	// Returns a QVV transforms with the rotation part normalized.
	//////////////////////////////////////////////////////////////////////////
	inline qvvf RTM_SIMD_CALL qvv_normalize(qvvf_arg0 input) RTM_NO_EXCEPT
	{
		const quatf rotation = quat_normalize(input.rotation);
		return qvv_set(rotation, input.translation, input.scale);
	}
}

RTM_IMPL_FILE_PRAGMA_POP
