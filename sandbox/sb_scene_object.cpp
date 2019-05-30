/*
 *  sb_object.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */


#include "sb_scene_object.h"
#include <sbstd/sb_assert.h>
#include "sb_container.h"
#include "sb_scene.h"

SB_META_DECLARE_OBJECT(Sandbox::DrawAttributes,Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::SceneObject, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::SceneObjectWithPosition, Sandbox::SceneObject)

namespace Sandbox {


	SceneObject::SceneObject() : m_parent(0) ,m_visible(true),m_order(0.0f){
	}
	
	SceneObject::~SceneObject() {
		sb_assert(m_parent==0);
	}
	
	void SceneObject::SetParent(Container* parent) {
		m_parent = parent;
	}
    
    Scene* SceneObject::GetScene() const {
        Container* c = GetParent();
        while (c) {
            Scene* s = meta::sb_dynamic_cast<Scene>(c);
            if (s) return s;
            c = c->GetParent();
        }
        return 0;
    }
    
    void SceneObject::RemoveFromScene() {
        Container* c = GetParent();
        if (c) {
            c->RemoveObject(SceneObjectPtr(this));
        }
    }
    
    void SceneObject::GlobalToLocalImpl(Vector2f& v) const {
        if (m_parent) {
            m_parent->GlobalToLocalImpl(v);
        }
    }
    Transform2d SceneObject::GetTransform() const {
        Transform2d tr;
        GetTransformImpl(tr);
        return tr;
    }
    Transform2d SceneObject::GetTransformTo(const SceneObject* root) const {
        Transform2d tr;
        GetTransformToImpl(root,tr);
        return tr;
    }
    void SceneObject::GetTransformImpl(Transform2d& v) const {
        if (m_parent) {
            m_parent->GetTransformImpl(v);
        }
    }
    void SceneObject::GetTransformToImpl(const SceneObject* root,Transform2d& tr) const {
        if (this == root) {
            return;
        }
        if (m_parent) {
            m_parent->GetTransformToImpl(root,tr);
        }
    }
    
    Vector2f SceneObject::LocalToGlobal(const Vector2f& v) const {
        Transform2d tr = GetTransform();
        return tr.transform(v);
    }
    
    Vector2f SceneObject::LocalTo(const SceneObject* root,const Vector2f& v) const {
        Transform2d tr = GetTransformTo(root);
        return tr.transform(v);
    }
    
    Vector2f SceneObject::GlobalToLocal(const Vector2f& v) const {
        Vector2f res = v;
        GlobalToLocalImpl(res);
        return res;
    }
    
    void SceneObject::SetOrder(float order) {
        m_order = order;
        if (m_parent) {
            m_parent->SortByOrder();
        }
    }

    Vector3f SceneObject::LocalToGlobal3d(const Vector3f& v) const {
        Transform3d tr = GetTransform3d();
        auto test_value = tr.transform(v);
        test_value.y *= -1.0f;
        return test_value;
    }

    Transform3d SceneObject::GetTransform3d() const {
        Transform3d tr;
        GetTransform3dImpl(tr);
        return tr;
    }
    
    void SceneObject::GetTransform3dImpl(Transform3d& tr) const {
        if (m_parent) {
            m_parent->GetTransform3dImpl(tr);
        }
    }
    
    Vector3f SceneObject::Global3dToLocal(const Vector3f& v) const {
        Vector3f res = v;
        Global3dToLocalImpl(res);
        return res;
    }
    
    void SceneObject::Global3dToLocalImpl(Vector3f& v) const {
        if (m_parent) {
            m_parent->Global3dToLocalImpl(v);
        }
    }
    
    
    void SceneObjectWithPosition::GlobalToLocalImpl(Vector2f& v) const {
        SceneObject::GlobalToLocalImpl(v);
        v -= m_pos;
    }
    
    void SceneObjectWithPosition::GetTransformImpl(Transform2d& tr) const {
        SceneObject::GetTransformImpl(tr);
        tr.translate(m_pos);
    }
    
    void SceneObjectWithPosition::GetTransformToImpl(const SceneObject* root, Transform2d& tr) const {
        SceneObject::GetTransformToImpl(root,tr);
        if (root != this)
            tr.translate(m_pos);
    }
    
    void SceneObjectWithPosition::GetTransform3dImpl(Transform3d& tr) const {
        SceneObject::GetTransform3dImpl(tr);
        tr.translate(Vector3f(m_pos));
    }
    
    void SceneObjectWithPosition::Global3dToLocalImpl(Vector3f& v) const {
        SceneObject::Global3dToLocalImpl(v);
        v -= Vector3f(m_pos);
    }
    
}
