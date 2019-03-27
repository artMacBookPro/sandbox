#include "sb_mygui_animated_layer.h"
#include "sb_mygui_render.h"
#include "sb_graphics.h"
#include "widgets/sb_mygui_animated_widget.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer, MyGUI::OverlappedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayerNode, MyGUI::LayerNode)

SB_META_DECLARE_OBJECT(Sandbox::mygui::Animated3dLayer, Sandbox::mygui::AnimatedLayer)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AnimatedLayer3dNode, Sandbox::mygui::AnimatedLayerNode)


namespace Sandbox {
    
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(AnimatedLayer)
        MYGUI_IMPL_TYPE_NAME(Animated3dLayer)
        MYGUI_IMPL_TYPE_NAME(AnimatedLayerNode)
        MYGUI_IMPL_TYPE_NAME(AnimatedLayer3dNode)
        
        AnimatedLayerProxy::AnimatedLayerProxy() : m_node(0) {
            
        }
        
        AnimatedLayerProxy::~AnimatedLayerProxy() {
            if (m_node) {
                m_node->resetProxy();
            }
        }
        
        void AnimatedLayerProxy::resetLayerNode(AnimatedLayerNode *node) {
            if (node == m_node) {
                m_node->resetProxy();
                m_node = 0;
            }
        }
        
        void AnimatedLayerProxy::setNodeOutOfDate() {
            if (m_node) {
                m_node->setOutOfDate();
            }
        }
        
        void AnimatedLayerProxy::attachLayerNode(AnimatedLayerNode *node) {
            if (m_node == node)
                return;
            if (m_node) {
                m_node->resetProxy();
            }
            m_node = node;
        }
    
        AnimatedLayerNode::AnimatedLayerNode( ILayerNode* _parent , AnimatedLayerProxy* proxy ) : MyGUI::LayerNode(_parent->getLayer(),_parent), m_proxy(proxy) {
            sb_assert(getLayer());
            if (m_proxy) {
                m_proxy->attachLayerNode(this);
            }
        }
        
        AnimatedLayerNode::AnimatedLayerNode( AnimatedLayer* _layer , MyGUI::Widget* _widget) : MyGUI::LayerNode(_layer,0), m_proxy(0) {
            sb_assert(getLayer());
            if (_widget) {
                AnimatedWidget* aw = _widget->castType<AnimatedWidget>(false);
                if (aw) {
                    m_proxy = aw;
                    aw->attachLayerNode(this);
                }
            }
          }
        
        AnimatedLayerNode::~AnimatedLayerNode() {
            if (m_proxy) {
                m_proxy->resetLayerNode(this);
            }
        }
        
        void AnimatedLayerNode::resetProxy() {
            m_proxy = 0;
        }
        
        void AnimatedLayerNode::detachLayerItem(MyGUI::ILayerItem* _root) {
            if (std::find(mLayerItems.begin(), mLayerItems.end(), _root)!=mLayerItems.end()) {
                Base::detachLayerItem(_root);
            }
        }
        
        void AnimatedLayerNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update) {
            if (m_proxy) {
                if (m_proxy->renderNodeToTarget(_target, this, _update)) {
                    return;
                }
            }
            Base::renderToTarget(_target, _update);
        }
        
        
        AnimatedLayer::AnimatedLayer() {
        
        }
        
        AnimatedLayer::~AnimatedLayer() {
        
        }
    
        MyGUI::ILayerNode* AnimatedLayer::createRootItemNode(MyGUI::Widget* widget) {
            // создаем рутовый айтем
            AnimatedLayerNode* node = new AnimatedLayerNode(this,widget);
            mChildItems.push_back(node);
            
            mOutOfDate = true;
            
            return node;
        }
        
    
        void AnimatedLayer::destroyRootItemNode(MyGUI::ILayerNode* _node) {
            Base::destroyRootItemNode(_node);
        }
        

        AnimatedLayer3dNode::AnimatedLayer3dNode(ILayerNode* _parent , AnimatedLayerProxy* proxy)
        :  AnimatedLayerNode(_parent, proxy)
        {
            
        }
        
        AnimatedLayer3dNode::AnimatedLayer3dNode(Animated3dLayer* _layer, MyGUI::Widget* _widget)
        : AnimatedLayerNode(_layer, _widget)
        {
            
        }
        
        AnimatedLayer3dNode::~AnimatedLayer3dNode()
        {
            
        }
            
        void AnimatedLayer3dNode::renderToTarget(MyGUI::IRenderTarget* _target, bool _update)
        {
            if (m_transform3D)
            {
                Graphics& g = *static_cast<RenderTargetImpl*>(_target)->graphics();
                Matrix4f old_v = g.GetViewMatrix();

                g.SetViewMatrix(old_v * m_transform3D->GetTransformMatrix());
                
                Base::renderToTarget(_target, _update);
                
                g.SetViewMatrix(old_v);
                
            }
            else
                Base::renderToTarget(_target, _update);
        }
        
        void AnimatedLayer3dNode::SetTransform3dModificator(const Transform3dModificatorPtr& ptr) {
            m_transform3D = ptr;
        }
        
        Transform3dModificatorPtr AnimatedLayer3dNode::GetTransform3dModificator() {
            if (!m_transform3D) {
                m_transform3D.reset(new Transform3dModificator());
            }
            return m_transform3D;
        }
        
        Animated3dLayer::Animated3dLayer()
        : m_scale_layer(1.0)
        {
        
        }
        
        Animated3dLayer::~Animated3dLayer() {
        
        }
        
        MyGUI::ILayerNode* Animated3dLayer::createRootItemNode(MyGUI::Widget* widget) {
            // создаем рутовый айтем
            AnimatedLayer3dNode* node = new AnimatedLayer3dNode(this,widget);
            mChildItems.push_back(node);
            
            mOutOfDate = true;
            
            return node;
        }
        
        void Animated3dLayer::renderToTarget(MyGUI::IRenderTarget* _target, bool _update)
        {
            if(m_modificator)
            {
                Graphics& g = *static_cast<RenderTargetImpl*>(_target)->graphics();
                Matrix4f prev_projection = g.GetProjectionMatrix();
                Matrix4f prev_view = g.GetViewMatrix();
                
                const Matrix4f& view_matrix = m_modificator->GetViewMatrix();
                const Matrix4f& projection_matrix = m_modificator->GetProjectionMatrix();
                
                Transform2d tr, prev_tr;
                prev_tr = g.GetTransform();
                tr.scale(1.0, -1.0); // -1.0f because opengl coord doesn't match window system
                
                
                Matrix4f scaled_matrix = view_matrix * Matrix4f::scale(m_scale_layer, m_scale_layer, m_scale_layer);
                
                g.SetTransform(tr);
                g.SetViewMatrix(scaled_matrix);
                g.SetProjectionMatrix(projection_matrix);
                
                Base::renderToTarget(_target, _update);
                
                g.SetViewMatrix(prev_view);
                g.SetProjectionMatrix(prev_projection);
                g.SetTransform(prev_tr);
            }
            else
                Base::renderToTarget(_target, _update);
            
        }
   
        void Animated3dLayer::SetViewProjection3dModificator(const ViewProjection3dModificatorPtr& ptr) {
            m_modificator = ptr;
        }
        
        ViewProjection3dModificatorPtr Animated3dLayer::GetViewProjection3dModificator()
        {
            if (!m_modificator) {
                m_modificator.reset(new ViewProjection3dModificator());
            }
            return m_modificator;
        }
        
}
}
