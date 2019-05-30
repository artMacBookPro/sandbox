//
//  sb_draw_modificator.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/17/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_draw_modificator__
#define __sr_osx__sb_draw_modificator__

#include "meta/sb_meta.h"
#include <sbstd/sb_vector.h>
#include "sb_color.h"
#include "sb_transform2d.h"
#include "sb_transform3d.h"
#include "sb_matrix4.h"

namespace Sandbox {
    
    class Graphics;
    
    
    
    class ColorModificator : public meta::object {
        SB_META_OBJECT
    private:
        Color   m_color;
    public:
        
        const Color& GetColor() const { return m_color; }
        void SetColor(const Color& c) { m_color = c; }
        float GetAlpha() const { return m_color.a; }
        void SetAlpha(float a) { m_color.a = a; }
        
        void Apply(Graphics& g) const;
    };
    typedef sb::intrusive_ptr<ColorModificator> ColorModificatorPtr;
    
    class TransformModificator : public meta::object {
        SB_META_OBJECT
    private:
        Vector2f	m_translate;
		float		m_scale_x;
		float		m_scale_y;
		float		m_angle;
        float       m_screw_x;
        Vector2f    m_origin;
    public:
        TransformModificator();
        
        void SetTranslate(const Vector2f& tr) { m_translate = tr; }
		const Vector2f& GetTranslate() const { return m_translate;}
        void SetOrigin(const Vector2f& tr) { m_origin = tr; }
        const Vector2f& GetOrigin() const { return m_origin;}
        
        void SetTranslateX(float x) { m_translate.x = x; }
        float GetTranslateX() const { return m_translate.x;}
        void SetTranslateY(float y) { m_translate.y = y; }
        float GetTranslateY() const { return m_translate.y;}
		void SetScale(float s) { m_scale_x = m_scale_y = s;}
        float GetScale() const { return (m_scale_x + m_scale_y)*0.5f; }
		void SetScaleX(float s) { m_scale_x = s;}
		float GetScaleX() const { return m_scale_x;}
		void SetScaleY(float s) { m_scale_y = s;}
		float GetScaleY() const { return m_scale_y;}
		void SetAngle(float a) { m_angle = a;}
		float GetAngle() const { return m_angle;}
        void SetScrewX(float a) { m_screw_x = a;}
        float GetScrewX() const { return m_screw_x;}

        void UnTransform(Vector2f& v) const;
        void Transform(Vector2f& v) const;
        void Apply(Graphics& g) const;
        void Apply(Transform2d& tr) const;
    };
    typedef sb::intrusive_ptr<TransformModificator> TransformModificatorPtr;
    
    class ViewProjection3dModificator : public meta::object {
        SB_META_OBJECT
    private:
        Matrix4f    m_view_matrix;
        Matrix4f    m_projection_matrix;
    public:
        ViewProjection3dModificator();
        
        void SetViewMatrix(const Matrix4f& m)           { m_view_matrix = m;            }
        const Matrix4f& GetViewMatrix() const           { return m_view_matrix;         }
        
        void SetProjectionMatrix(const Matrix4f& m)     { m_projection_matrix = m;      }
        const Matrix4f& GetProjectionMatrix() const     { return m_projection_matrix;   }
    };
    typedef sb::intrusive_ptr<ViewProjection3dModificator> ViewProjection3dModificatorPtr;
    
    class Transform3dModificator : public meta::object {
        SB_META_OBJECT
    private:
        Vector3f    m_translate;
        Vector3f    m_scale;
        Vector3f    m_rotate;
        Matrix4f    m_matrix;
    public:
        Transform3dModificator();
        Transform3dModificator(const TransformModificatorPtr& transform_2d);
        void SetTranslateV3(const Vector3f& tr) { m_translate = tr; update_matrix();}
        const Vector3f& GetTranslateV3() const { return m_translate;}
        void SetTranslate(const Vector2f& tr) { m_translate.x = tr.x; m_translate.y = tr.y; update_matrix();}
        Vector2f GetTranslate() const { return Vector2f(m_translate.x, m_translate.y);}
        
        void SetScaleV3(const Vector3f& s) { m_scale = s; update_matrix();}
        const Vector3f& GetScaleV3() const { return m_scale; }
        void SetScale(float s) { m_scale.x = m_scale.y = s; update_matrix();}
        float GetScale() const { return (m_scale.x + m_scale.y) * 0.5f; }
        
        void SetRotate(const Vector3f& s) { m_rotate = s; update_matrix();}
        const Vector3f& GetRotate() const { return m_rotate; }
        
        void SetTransformMatrix(const Matrix4f& m) {m_matrix = m;}
        const Matrix4f& GetTransformMatrix() const {return m_matrix;}
        
        void Apply(Transform3d& tr) const;
        void UnTransform(Vector3f& v) const;
    private:
        void update_matrix();
    };
    typedef sb::intrusive_ptr<Transform3dModificator> Transform3dModificatorPtr;
}

#endif /* defined(__sr_osx__sb_draw_modificator__) */
