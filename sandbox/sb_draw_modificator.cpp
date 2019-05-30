//
//  sb_draw_modificator.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_draw_modificator.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::ColorModificator, meta::object)
SB_META_DECLARE_OBJECT(Sandbox::TransformModificator, meta::object)
SB_META_DECLARE_OBJECT(Sandbox::ViewProjection3dModificator, meta::object)
SB_META_DECLARE_OBJECT(Sandbox::Transform3dModificator, meta::object)

namespace Sandbox {
    
    
    
    void ColorModificator::Apply(Sandbox::Graphics &g) const {
        g.SetColor(g.GetColor()*m_color);
    }
   
    TransformModificator::TransformModificator() : m_scale_x(1.0f),m_scale_y(1.0f),m_angle(0.0f),m_screw_x(0.0f) {
    }
    
    void TransformModificator::Apply(Graphics &g) const {
        Transform2d tr = g.GetTransform();
        Apply(tr);
        g.SetTransform(tr);
    }
    void TransformModificator::Apply(Transform2d& tr) const {
        tr.translate(m_origin);
        tr.translate(m_translate);
        if (m_screw_x != 0.0f)
            tr.screw_x(m_screw_x);
        tr.rotate(m_angle).scale(m_scale_x,m_scale_y);
        tr.translate(-m_origin);
    }
    void TransformModificator::UnTransform(Vector2f& v) const {
        Transform2d tr;
        Apply(tr);
        tr.inverse();
        v = tr.transform(v);
    }
    void TransformModificator::Transform(Vector2f& v) const {
        Transform2d tr;
        Apply(tr);
        v = tr.transform(v);
    }
    
    ViewProjection3dModificator::ViewProjection3dModificator()
    {
        m_view_matrix = m_projection_matrix = Matrix4f::identity();
    }
    
    Transform3dModificator::Transform3dModificator()
    :   m_scale(1.0, 1.0, 1.0),
        m_rotate(0.0, 0.0, 0.0),
        m_translate(0.0, 0.0, 0.0)
    {
        m_matrix = Matrix4f::identity();
    }
    
    Transform3dModificator::Transform3dModificator(const TransformModificatorPtr& tr)
    :
        m_rotate(0.0, 0.0, 0.0)
    {
        m_scale = Vector3f(tr->GetScaleX(), tr->GetScaleY(), 1.0f);
        
        m_translate = Vector3f(tr->GetTranslateX(), tr->GetTranslateY(), 0.0f);
        update_matrix();
    }
    
    void Transform3dModificator::update_matrix()  {

        m_matrix = Matrix4f::translate(m_translate) *
                    Matrix4f::rotate(m_rotate.x, m_rotate.y, m_rotate.z, EULER_ORDER_XYZ) *
                    Matrix4f::scale(m_scale.x, m_scale.y, m_scale.z);
    }
    
    void Transform3dModificator::Apply(Transform3d& tr) const {
        tr.translate(m_translate);
        tr.rotate(m_rotate);
        tr.scale(m_scale);
    }
    
    void Transform3dModificator::UnTransform(Vector3f& v) const
    {
        Transform3d tr;
        Apply(tr);
        v = tr.un_transform(v);
    }
    
}
