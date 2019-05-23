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
	
	class ContainerTransformMVP : public Container {
        SB_META_OBJECT
   public:
		ContainerTransformMVP();
		~ContainerTransformMVP() SB_OVERRIDE;
		
		void Draw(Graphics& g) const SB_OVERRIDE;
        
        virtual void GlobalToLocalImpl(Vector2f& v) const SB_OVERRIDE;
        virtual void GetTransformImpl(Transform2d& tr) const SB_OVERRIDE;
        virtual void GetTransform3dImpl(Transform3d& tr) const SB_OVERRIDE;
        
        const Rectf& GetViewRect() const { return m_view_rect; }
        void SetViewRect(const Rectf& r) { m_view_rect = r;};
        
        //
        Vector3f ScreenPosToWorld3d(const Vector2f& ndc_pos, const Vector3f& object_world_pos, const Vector3f& object_normal = Vector3f(0.0, 0.0, 0.1f)) const;
        void SetViewProjection3dModificator(const ViewProjection3dModificatorPtr& ptr);
        ViewProjection3dModificatorPtr GetViewProjection3dModificator();
    protected:
        //virtual void GetTransform3dImpl(Transform3d& tr) const SB_OVERRIDE;
	private:
        Rectf       m_view_rect;
        ViewProjection3dModificatorPtr m_vp_modificator;
	};
	typedef sb::intrusive_ptr<ContainerTransformMVP> ContainerTransformMVPPtr;
    
    class ContainerTransform3d : public Container {
        SB_META_OBJECT
    public:
        ContainerTransform3d();
        ~ContainerTransform3d() SB_OVERRIDE;
        
        void Draw(Graphics& g) const SB_OVERRIDE;
        virtual void GlobalToLocalImpl(Vector2f& v) const SB_OVERRIDE;
        virtual void GetTransformImpl(Transform2d& tr) const SB_OVERRIDE;
        virtual void Global3dToLocalImpl(Vector3f& v) const SB_OVERRIDE;
        
        void SetTransform3dModificator(const Transform3dModificatorPtr& ptr);
        Transform3dModificatorPtr GetTransform3dModificator();
    protected:
        virtual void GetTransform3dImpl(Transform3d& tr) const SB_OVERRIDE;
    private:
        Transform3dModificatorPtr m_transform3d;
    };
    typedef sb::intrusive_ptr<ContainerTransform3d> ContainerTransform3dPtr;
}

#endif /*SB_CONTAINER_TRANSFORM3D_H*/
