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


SB_META_DECLARE_OBJECT(Sandbox::ContainerTransformMVP, Sandbox::Container)
SB_META_DECLARE_OBJECT(Sandbox::ContainerTransform3d, Sandbox::Container)

namespace Sandbox {
    
	ContainerTransformMVP::ContainerTransformMVP()  {
	}
	
	ContainerTransformMVP::~ContainerTransformMVP() {
	}
    
    Vector3f ContainerTransformMVP::ScreenPosToWorld3d(const Vector2f& ndc_pos, const Vector3f& object_world_pos, const Vector3f& object_normal) const
    {
        Vector3f result;
        if (!m_vp_modificator)
            return result;
        
        const Matrix4f& view_matrix = m_vp_modificator->GetViewMatrix();
        const Matrix4f& projection_matrix = m_vp_modificator->GetProjectionMatrix();
    
        Vector3f origin_camera(view_matrix.matrix[12], view_matrix.matrix[13], view_matrix.matrix[14]);
        
        Matrix4f inverse_vp = projection_matrix * view_matrix;
        inverse_vp.inverse();
        
        Vector4f np = inverse_vp * Vector4f(ndc_pos.x, ndc_pos.y, -1.0f, 1.0f);
        Vector4f fp = inverse_vp * Vector4f(ndc_pos.x, ndc_pos.y, 1.0f, 1.0f);
        np *= (1.0f / np.w);
        fp *= (1.0f / fp.w);
        
        Vector3f ray_np(np);
        Vector3f ray_fp(fp);
        
        
        float denom = Vector3f::dot(object_normal, ray_fp - ray_np);
        bool is_perpendicular = denom == 0.f;
        if (is_perpendicular)
            return result;
        
        float numer = Vector3f::dot(object_normal, object_world_pos - ray_np);
        result = interpolate(ray_np, ray_fp, numer / denom);
        return result;
    }
	
	void ContainerTransformMVP::Draw(Graphics& g) const {
        if (m_vp_modificator)
        {
            Matrix4f prev_projection = g.GetProjectionMatrix();
            Matrix4f prev_view = g.GetViewMatrix();
            Transform2d prev_tr = g.GetTransform();
            const Matrix4f& view_matrix = m_vp_modificator->GetViewMatrix();
            const Matrix4f& projection_matrix = m_vp_modificator->GetProjectionMatrix();
            
            Transform2d tr = prev_tr;
            
            tr.m.matrix[3] *= -1.0; // because our game has render from top to bottom but opengl from center to top
            tr.m.matrix[0] = Sandbox::sb_copysign(1.0, tr.m.matrix[0]);
            tr.m.matrix[3] = Sandbox::sb_copysign(1.0, tr.m.matrix[3]);
            
            Matrix4f scaled_matrix = view_matrix * Matrix4f::scale(std::fabs(prev_tr.m.matrix[0]), std::fabs(prev_tr.m.matrix[3]), std::fabs(prev_tr.m.matrix[0]));
            
            g.SetTransform(tr);
            g.SetProjectionMatrix(projection_matrix);
            g.SetViewMatrix(scaled_matrix);
            
            Container::Draw(g);
            
            g.SetViewMatrix(prev_view);
            g.SetProjectionMatrix(prev_projection);
            g.SetTransform(prev_tr);
        }
        else
        {
            Container::Draw(g);
        }
	}
    
    void ContainerTransformMVP::SetViewProjection3dModificator(const ViewProjection3dModificatorPtr& ptr) {
        m_vp_modificator = ptr;
    }
    
    ViewProjection3dModificatorPtr ContainerTransformMVP::GetViewProjection3dModificator() {
        if (!m_vp_modificator) {
            m_vp_modificator.reset(new ViewProjection3dModificator());
        }
        return m_vp_modificator;
    }
    
    void ContainerTransformMVP::GlobalToLocalImpl(Vector2f& v) const {
        Container::GlobalToLocalImpl(v);
        //v.y *= -1.0; // special case because in 3d scene global coord shoulb be from top to bottom too
        
    }
    
    void ContainerTransformMVP::GetTransformImpl(Transform2d& tr) const {
        Container::GetTransformImpl(tr);
        //tr.m.matrix[3] = tr.m.matrix[3] * -1.0f; // inverse y coord because we work in space where Y coord from top to bottom
        //tr.v.y *= -1.f;
    }
    
    void ContainerTransformMVP::GetTransform3dImpl(Transform3d& tr) const {
        Container::GetTransform3dImpl(tr);
        //tr.scale(1.0, -1.0);
    }
    
    ContainerTransform3d::ContainerTransform3d()
    {
    }
    
    ContainerTransform3d::~ContainerTransform3d()
    {
    }
    
    void ContainerTransform3d::GlobalToLocalImpl(Vector2f& v) const {
        Container::GlobalToLocalImpl(v);
        
        if (m_transform3d)
        {
            // matrix rotation is not similar to angle rotation transform2d and we shouldn't use coord transformation with rotate
            auto rotate_v = m_transform3d->GetRotate();
            Matrix4f transform_matrix = m_transform3d->GetTransformMatrix();
            transform_matrix = transform_matrix * Matrix4f::rotate(rotate_v.x, rotate_v.y, rotate_v.z, EULER_ORDER_XYZ).inverse();
            auto untransform_m = transform_matrix.inverse();
            auto new_pt = untransform_m * Vector4f(v.x, v.y, 0.0, 1.0);
            v.x = new_pt.x;
            v.y = new_pt.y;
        }
    }
    
    void ContainerTransform3d::GetTransformImpl(Transform2d& tr) const {
        Container::GetTransformImpl(tr);
        if (m_transform3d)
        {
            const auto& scale_v = m_transform3d->GetScaleV3();
            const auto& translate_v = m_transform3d->GetTranslateV3();
            //const auto& rotate_v = m_transform3d->GetRotate();
            
            tr.translate(translate_v.x, translate_v.y);
            tr.scale(scale_v.x, scale_v.y);
            // matrix rotation is not similar to angle rotation transform2d and we shouldn't use coord transformation with rotate
            //tr.rotate(rotate_v.z);
        }
    }
    
    void ContainerTransform3d::Draw(Graphics& g) const {
        Matrix4f old_v = g.GetViewMatrix();
        Transform2d tr = g.GetTransform();
        Transform2d prev_tr = tr;
        
        tr.m.matrix[0] = Sandbox::sb_copysign(1.0, tr.m.matrix[0]);
        tr.m.matrix[3] = Sandbox::sb_copysign(1.0, tr.m.matrix[3]);
            
        Matrix4f model_matrix = m_transform3d ? m_transform3d->GetTransformMatrix() : Matrix4f::identity();
        model_matrix.matrix[13] *= -1.0; // it's for normal mapping 2D and 3D Y coord
        
        Matrix4f scaled_matrix = model_matrix * Matrix4f::scale(std::fabs(prev_tr.m.matrix[0]), std::fabs(prev_tr.m.matrix[3]), std::fabs(prev_tr.m.matrix[0]));
        
        g.SetTransform(tr);
        g.SetViewMatrix(old_v * scaled_matrix);
        
        Container::Draw(g);
        
        g.SetViewMatrix(old_v);
        g.SetTransform(prev_tr);
    }
    
    void ContainerTransform3d::SetTransform3dModificator(const Transform3dModificatorPtr& ptr) {
        m_transform3d = ptr;
    }
    
    Transform3dModificatorPtr ContainerTransform3d::GetTransform3dModificator() {
        if (!m_transform3d) {
            m_transform3d.reset(new Transform3dModificator());
        }
        return m_transform3d;
    }
    
    void ContainerTransform3d::GetTransform3dImpl(Transform3d& tr) const {
        Container::GetTransform3dImpl(tr);
        if (m_transform3d)
        {
            m_transform3d->Apply(tr);
        }
    }
    
    void ContainerTransform3d::Global3dToLocalImpl(Vector3f& v) const {
        Container::Global3dToLocalImpl(v);
        
        if(m_transform3d)
            m_transform3d->UnTransform(v);
    }
}
