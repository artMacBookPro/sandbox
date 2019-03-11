/*
 *  sb_container_transform3d.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_TRANSFORM3D_H
#define SB_CONTAINER_TRANSFORM3D_H

#include "sb_container.h"
#include "sb_matrix4.h"
#include "sb_rect.h"

namespace Sandbox {
	
	class ContainerTransform3d : public Container {
        SB_META_OBJECT
   public:
		ContainerTransform3d();
		~ContainerTransform3d() SB_OVERRIDE;
		
		void Draw(Graphics& g) const SB_OVERRIDE;
		
		void SetProjectionMatrix(const Matrix4f& m) { m_projection = m; }
		const Matrix4f& GetProjectionMatrix() const { return m_projection;}
		void SetViewMatrix(const Matrix4f& m) { m_view = m; }
		const Matrix4f& GetViewMatrix() const { return m_view;}
        
        virtual void GlobalToLocalImpl(Vector2f& v) const SB_OVERRIDE;
        virtual void GetTransformImpl(Transform2d& tr) const SB_OVERRIDE;
        
        const Rectf& GetViewRect() const { return m_view_rect; }
        void SetViewRect(const Rectf& r) { m_view_rect = r;};
	private:
		Matrix4f	m_projection;
		Matrix4f	m_view;
        Rectf       m_view_rect;
	};
	typedef sb::intrusive_ptr<ContainerTransform3d> ContainerTransform3dPtr;
    
    class ContainerTransform3dPerspective : public ContainerTransform3d {
        SB_META_OBJECT
    public:
        ContainerTransform3dPerspective();
        ~ContainerTransform3dPerspective() SB_OVERRIDE;
        
        void Draw(Graphics& g) const SB_OVERRIDE;
        virtual void GlobalToLocalImpl(Vector2f& v) const SB_OVERRIDE;
        virtual void GetTransformImpl(Transform2d& tr) const SB_OVERRIDE;
        
        void SetTransform3dModificator(const Transform3dModificatorPtr& ptr);
        Transform3dModificatorPtr GetTransform3dModificator();
    private:
        Transform3dModificatorPtr m_transform3D;
    };
    typedef sb::intrusive_ptr<ContainerTransform3dPerspective> ContainerTransform3dPerspectivePtr;
}

#endif /*SB_CONTAINER_TRANSFORM3D_H*/
