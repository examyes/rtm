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
#include "rtm/quatd.h"
#include "rtm/vector4d.h"
#include "rtm/affine_matrix_64.h"

namespace rtm
{
	constexpr qvvd transform_set(const quatd& rotation, const vector4d& translation, const vector4d& scale)
	{
		return qvvd{ rotation, translation, scale };
	}

	inline qvvd transform_identity_64()
	{
		return transform_set(quat_identity_64(), vector_zero_64(), vector_set(1.0));
	}

	inline qvvd transform_cast(const qvvf& input)
	{
		return qvvd{ quat_cast(input.rotation), vector_cast(input.translation), vector_cast(input.scale) };
	}

	// Multiplication order is as follow: local_to_world = transform_mul(local_to_object, object_to_world)
	// NOTE: When scale is present, multiplication will not properly handle skew/shear, use affine matrices instead
	inline qvvd transform_mul(const qvvd& lhs, const qvvd& rhs)
	{
		const vector4d min_scale = vector_min(lhs.scale, rhs.scale);
		const vector4d scale = vector_mul(lhs.scale, rhs.scale);

		if (vector_any_less_than3(min_scale, vector_zero_64()))
		{
			// If we have negative scale, we go through a matrix
			const AffineMatrix_64 lhs_mtx = matrix_from_transform(lhs);
			const AffineMatrix_64 rhs_mtx = matrix_from_transform(rhs);
			AffineMatrix_64 result_mtx = matrix_mul(lhs_mtx, rhs_mtx);
			result_mtx = matrix_remove_scale(result_mtx);

			const vector4d sign = vector_sign(scale);
			result_mtx.x_axis = vector_mul(result_mtx.x_axis, vector_mix_xxxx(sign));
			result_mtx.y_axis = vector_mul(result_mtx.y_axis, vector_mix_yyyy(sign));
			result_mtx.z_axis = vector_mul(result_mtx.z_axis, vector_mix_zzzz(sign));

			const quatd rotation = quat_from_matrix(result_mtx);
			const vector4d translation = result_mtx.w_axis;
			return transform_set(rotation, translation, scale);
		}
		else
		{
			const quatd rotation = quat_mul(lhs.rotation, rhs.rotation);
			const vector4d translation = vector_add(quat_rotate(rhs.rotation, vector_mul(lhs.translation, rhs.scale)), rhs.translation);
			return transform_set(rotation, translation, scale);
		}
	}

	// Multiplication order is as follow: local_to_world = transform_mul(local_to_object, object_to_world)
	inline qvvd transform_mul_no_scale(const qvvd& lhs, const qvvd& rhs)
	{
		const quatd rotation = quat_mul(lhs.rotation, rhs.rotation);
		const vector4d translation = vector_add(quat_rotate(rhs.rotation, lhs.translation), rhs.translation);
		return transform_set(rotation, translation, vector_set(1.0));
	}

	inline vector4d transform_position(const qvvd& lhs, const vector4d& rhs)
	{
		return vector_add(quat_rotate(lhs.rotation, vector_mul(lhs.scale, rhs)), lhs.translation);
	}

	inline vector4d transform_position_no_scale(const qvvd& lhs, const vector4d& rhs)
	{
		return vector_add(quat_rotate(lhs.rotation, rhs), lhs.translation);
	}

	inline qvvd transform_inverse(const qvvd& input)
	{
		const quatd inv_rotation = quat_conjugate(input.rotation);
		const vector4d inv_scale = vector_reciprocal(input.scale);
		const vector4d inv_translation = vector_neg(quat_rotate(inv_rotation, vector_mul(input.translation, inv_scale)));
		return transform_set(inv_rotation, inv_translation, inv_scale);
	}

	inline qvvd transform_inverse_no_scale(const qvvd& input)
	{
		const quatd inv_rotation = quat_conjugate(input.rotation);
		const vector4d inv_translation = vector_neg(quat_rotate(inv_rotation, input.translation));
		return transform_set(inv_rotation, inv_translation, vector_set(1.0));
	}

	inline qvvd transform_normalize(const qvvd& input)
	{
		const quatd rotation = quat_normalize(input.rotation);
		return transform_set(rotation, input.translation, input.scale);
	}
}