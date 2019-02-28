/*
 *  sb_container_transform3d.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 11.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_container_transform3d.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ContainerTransform3d, Sandbox::Container)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransform3dPerspective, Sandbox::ContainerTransform3d)

double
_copysign (double x, double y)
{
    return (signbit (x) != signbit (y) ? - x : x);
}

namespace Sandbox {

	ContainerTransform3d::ContainerTransform3d()  {
		m_projection = Matrix4f::identity();
		m_view = Matrix4f::identity();
	}
	
	ContainerTransform3d::~ContainerTransform3d() {
	}
	
	void ContainerTransform3d::Draw(Graphics& g) const {
		Matrix4f old_p = g.GetProjectionMatrix();
		Matrix4f old_v = g.GetViewMatrix();
        Transform2d tr = g.GetTransform();
        Transform2d prev_tr = tr;
        
        tr.m.matrix[0] = _copysign(1.0, tr.m.matrix[0]);
        tr.m.matrix[3] = _copysign(1.0, tr.m.matrix[3]);
        
        Matrix4f scaled_matrix = m_view * Matrix4f::scale(std::fabs(prev_tr.m.matrix[0]), std::fabs(prev_tr.m.matrix[3]), std::fabs(prev_tr.m.matrix[0]));
        
        g.SetTransform(tr);
        g.SetProjectionMatrix(m_projection);
        g.SetViewMatrix(scaled_matrix);

		Container::Draw(g);
        
        g.SetViewMatrix(old_v);
        g.SetProjectionMatrix(old_p);
        g.SetTransform(prev_tr);
	}
    
    void ContainerTransform3d::GlobalToLocalImpl(Vector2f& v) const {
        Container::GlobalToLocalImpl(v);
        return;
// Transformation of normalized coordinates between -1 and 1
//        in[0]=(winx-(float)viewport[0])/(float)viewport[2]*2.0-1.0;
//        in[1]=(winy-(float)viewport[1])/(float)viewport[3]*2.0-1.0;
//        in[2]=2.0*winz-1.0;
//        in[3]=1.0;
        
        Vector4f normolized_coord((v.x - m_view_rect.x) / m_view_rect.w * 2.f - 1.f,
                                  (m_view_rect.h + v.y) / m_view_rect.h * 2.f - 1.f,
                                  0.0,
                                  1.f
                                  );
        v.x = normolized_coord.x * m_view_rect.w * 0.5f;
        v.y =  (normolized_coord.y * m_view_rect.h * 0.5f);

        
        auto mvp = m_projection * m_view;
        Matrix4f inv_matrix = mvp.inverted();
        auto out_pt = inv_matrix * normolized_coord;
        Vector4f local_pt;
        if(out_pt.w != 0.0f)
        {
            out_pt.w = 1.0f / out_pt.w;
            local_pt.x = out_pt.x * out_pt.w;
            local_pt.y = out_pt.y * out_pt.w;
            local_pt.z = out_pt.z * out_pt.w;
            local_pt.w = 1.0f;
        }
        auto test_local = mvp * local_pt;
        Vector2f correct_pt = Vector2f(v.x / m_view_rect.w, (m_view_rect.h - v.y) / m_view_rect.h);
        Vector4f newV = Vector4f(correct_pt.x * 2.f - 1.f, correct_pt.y * 2.f - 1.f, 0.0, 1);
        Vector2f pt_in_3d_world(newV.x * m_view_rect.w / 2.0f, newV.y * m_view_rect.h / 2.0f);
        Vector4f vec_in_3d(pt_in_3d_world.x, pt_in_3d_world.y, 0.0, 1);
        
        auto m0 = Matrix4f::ortho(0.0f,
                                  m_view_rect.w,
                                  m_view_rect.h
                                  ,0.0f,-10.0f,10.0f);
        auto temp_view = Matrix4f::translate(568.0, 0, 0);
        auto mvp_ortho = m0 * temp_view;
        auto m02 = mvp_ortho * Vector4f(0.0, 0.0, 0.0, 1); // NDC point
        auto m002 = mvp_ortho.inverted() * Vector4f(0.0, 0.0, 0.0, 1);
        auto test_m002 = mvp_ortho *  m002;
        
        
        Matrix4f scaled_matrix = m_projection * m_view;
        auto sss = scaled_matrix;
        auto _m02 = scaled_matrix * vec_in_3d;
        auto _m02_inv = scaled_matrix.inverted() * vec_in_3d;
        auto check_m02 = scaled_matrix * _m02_inv;
        auto _m022 = scaled_matrix * Vector4f(568.0, 320.0, 0.0, 1);
        auto _m0222 = scaled_matrix * Vector4f(-284.0, 160.0, 0.0, 1);
        
        //m02 = m02.inverted();
        
        
        auto m03 = m02 * newV;
        
        auto m_tep = m0 * Matrix4f::translate(300, 0, 0);
        auto check = m_tep * m03;
        
        
        auto m2 = m_view;
        auto m3 = m2.inverted();
        auto m4 = m3 * newV;
        auto m5 = m_projection * m_view;
        
        auto t = Vector4f(10000, 0.0, 0.0, 1.0);
        auto t2 = m5 * t;
        auto t3 = m0 * t2;
        //auto t3 = t2 * m_tep;
        Vector4f test_point;
        if (_m02_inv.w != 0.0)
        {
            _m02_inv.w = 1.0f / _m02_inv.w;
            test_point = Vector4f(_m02_inv.x * _m02_inv.w, _m02_inv.y * _m02_inv.w, _m02_inv.z * _m02_inv.w, 1.0);
        }
        auto m6 = scaled_matrix * test_point;
        if (m6.w != 0.0)
        {
            m6.x /= m6.w;
            m6.y /= m6.w;
            m6.z /= m6.w;
        }
        auto m7 = m6 * m0;
        
        int a = 0;
        
        //vec4 clipSpacePos = projectionMatrix * (viewMatrix * vec4(point3D, 1.0));
        //vec3 ndcSpacePos = clipSpacePos.xyz / clipSpacePos.w;
        //vec2 windowSpacePos = vec2( ((ndcSpacePos.x + 1.0) / 2.0) * viewSize.x + viewOffset.x, ((1.0 - ndcSpacePos.y) / 2.0) * viewSize.y + viewOffset.y )
    }
    
    ContainerTransform3dPerspective::ContainerTransform3dPerspective()
    {
    }
    
    ContainerTransform3dPerspective::~ContainerTransform3dPerspective()
    {
    }
    void ContainerTransform3dPerspective::GlobalToLocalImpl(Vector2f& v) const {
        Container::GlobalToLocalImpl(v);
        return;
        if (m_transform3D)
        {
            auto untransform_m = m_transform3D->GetTransformMatrix().inverted();
            auto new_pt = untransform_m * Vector4f(v.x, v.y, 0.0, 1.0);
            v.x = new_pt.x;
            v.y = new_pt.y;
        }
    }
    
    void ContainerTransform3dPerspective::Draw(Graphics& g) const {
        Matrix4f old_v = g.GetViewMatrix();
        Transform2d tr = g.GetTransform();
        Transform2d prev_tr = tr;
        
        tr.m.matrix[0] = _copysign(1.0, tr.m.matrix[0]);
        tr.m.matrix[3] = _copysign(1.0, tr.m.matrix[3]);
            
        Matrix4f model_matrix = m_transform3D ? m_transform3D->GetTransformMatrix() : Matrix4f::identity();
        
        Matrix4f scaled_matrix = model_matrix * Matrix4f::scale(std::fabs(prev_tr.m.matrix[0]), std::fabs(prev_tr.m.matrix[3]), std::fabs(prev_tr.m.matrix[0]));
        
        Matrix4f model_view = GetViewMatrix() * scaled_matrix;
        
        g.SetTransform(tr);
        g.SetViewMatrix(old_v * model_view);
        
        Container::Draw(g);
        
        g.SetViewMatrix(old_v);
        g.SetTransform(prev_tr);
    }
    
    void ContainerTransform3dPerspective::SetTransform3dModificator(const Transform3dModificatorPtr& ptr) {
        m_transform3D = ptr;
    }
    
    Transform3dModificatorPtr ContainerTransform3dPerspective::GetTransform3dModificator() {
        if (!m_transform3D) {
            m_transform3D.reset(new Transform3dModificator());
        }
        return m_transform3D;
    }
}
