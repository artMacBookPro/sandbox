/*
 *  sb_Transform3d.h
 *  SR
 *
 *  Created by Андрей Куницын on 07.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_TRANSFORM3D_H
#define SB_TRANSFORM3D_H

#include "sb_transform2d.h"
#include "sb_matrix4.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Sandbox {
	
    struct Transform3d {
        Vector3f    m_translate;
        Vector3f    m_scale;
        Vector3f    m_rotate;
        
        Transform3d() : m_scale(1.0, 1.0, 1.0),
                        m_rotate(0.0, 0.0, 0.0),
                        m_translate(0.0, 0.0, 0.0)
                        {
                        }
        
        Transform3d(const Transform3d& other) = default;
        Transform3d& operator = (const Transform3d& other) = default;
        void reset() {
            m_scale = Vector3f(1.0, 1.0, 1.0);
            m_rotate = Vector3f(0.0, 0.0, 0.0);
            m_translate = Vector3f(0.0, 0.0, 0.0);
        }
		
        Transform3d& translate(const Vector3f& pos) {
            m_translate += pos;
            return *this;
        }
        
        // radians
        Transform3d& rotate(const Vector3f& dir) {
            m_rotate += dir;
            return *this;
        }
        
        Transform3d& scale(float s) {
            m_scale *= s;
            return *this;
        }
        Transform3d& scale(float sx,float sy) {
            m_scale *= Vector3f(sx, sy, 1.0);
            return *this;
        }
        Transform3d& scale(const Vector3f& s) {
            m_scale *= s;
            return *this;
        }
		
        Vector3f transform(const Vector3f& vert) const {
            auto test_result = get_matrix() * Vector4f(vert.x, vert.y, vert.z, 1.0);
            return Vector3f(test_result);
        }
        
        Vector3f un_transform(const Vector3f& vert) const {
            auto test_result = get_matrix().inverted() * Vector4f(vert.x, vert.y, vert.z, 1.0);
            return Vector3f(test_result);
        }
		
    private:
        Matrix4f get_matrix() const {
            
            return Matrix4f::translate(m_translate) *
            Matrix4f::rotate(m_rotate.x, m_rotate.y, m_rotate.z, EULER_ORDER_XYZ) *
            Matrix4f::scale(m_scale.x, m_scale.y, m_scale.z);
        }
    };
	
}

#endif // SB_TRANSFORM3D_H
