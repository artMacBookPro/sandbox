#ifndef _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_
#define _SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_

#include "MyGUI_OverlappedLayer.h"
#include "MyGUI_LayerNode.h"
#include "MyGUI_ILayer.h"
#include "sb_container.h"

namespace Sandbox {
    class ViewProjection3dModificator;
    typedef sb::intrusive_ptr<ViewProjection3dModificator> ViewProjection3dModificatorPtr;
    
    class Transform3dModificator;
    typedef sb::intrusive_ptr<Transform3dModificator> Transform3dModificatorPtr;
}

namespace Sandbox {
    
    namespace mygui {
        
        class AnimatedLayerNode;
        
        class AnimatedLayerProxy {
        private:
            AnimatedLayerNode*  m_node;
        public:
            AnimatedLayerProxy();
            virtual ~AnimatedLayerProxy();
            virtual bool renderNodeToTarget(MyGUI::IRenderTarget* rt,MyGUI::LayerNode* node,bool update) = 0;
            void attachLayerNode(AnimatedLayerNode* node);
            void resetLayerNode(AnimatedLayerNode* node);
            void setNodeOutOfDate();
            AnimatedLayerNode* getAnimatedLayerNode() { return m_node; }
        };
        
        class AnimatedLayer;
    
        class AnimatedLayerNode : public MyGUI::LayerNode {
            MYGUI_RTTI_DERIVED( AnimatedLayerNode )
        public:
            explicit AnimatedLayerNode(ILayerNode* _parent , AnimatedLayerProxy* proxy);
            explicit AnimatedLayerNode(AnimatedLayer* _layer, MyGUI::Widget* _widget);
            virtual ~AnimatedLayerNode();
            
            virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update);
            
            virtual void detachLayerItem(MyGUI::ILayerItem* _root);
            
            void resetProxy();
            void setOutOfDate() { mOutOfDate = true; }
        protected:
            AnimatedLayerProxy* m_proxy;
        };
        
        class AnimatedLayer : public MyGUI::OverlappedLayer {
            MYGUI_RTTI_DERIVED( AnimatedLayer )
        
        public:
            AnimatedLayer();
            virtual ~AnimatedLayer();
        
            virtual MyGUI::ILayerNode* createRootItemNode(MyGUI::Widget* widget) SB_OVERRIDE;
            virtual void destroyRootItemNode(MyGUI::ILayerNode* _node) SB_OVERRIDE;
            
        };
        
        class Animated3dLayer;
        
        class AnimatedLayer3dNode : public AnimatedLayerNode {
            MYGUI_RTTI_DERIVED( AnimatedLayer3dNode )
            explicit AnimatedLayer3dNode(ILayerNode* _parent , AnimatedLayerProxy* proxy);
            explicit AnimatedLayer3dNode(Animated3dLayer* _layer, MyGUI::Widget* _widget);
            virtual ~AnimatedLayer3dNode();
            
            virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update) SB_OVERRIDE;
            
            void SetTransform3dModificator(const Transform3dModificatorPtr& ptr);
            Transform3dModificatorPtr GetTransform3dModificator();
        private:
            Transform3dModificatorPtr m_transform3D;
        };
        

        class Animated3dLayer : public AnimatedLayer {
            MYGUI_RTTI_DERIVED( Animated3dLayer )
            
        public:
            Animated3dLayer();
            virtual ~Animated3dLayer();
            
            virtual void renderToTarget(MyGUI::IRenderTarget* _target, bool _update) SB_OVERRIDE;
            virtual MyGUI::ILayerNode* createRootItemNode(MyGUI::Widget* widget) SB_OVERRIDE;
            
            void SetViewProjection3dModificator(const ViewProjection3dModificatorPtr& ptr);
            ViewProjection3dModificatorPtr GetViewProjection3dModificator();
            
            void SetScaleLayer(float scale) { m_scale_layer = scale;};
            float GetScaleLayer() const { return m_scale_layer; };
            
        private:
            ViewProjection3dModificatorPtr m_modificator;
            float m_scale_layer;
        };
    }
}

#endif /*_SB_MYGUI_ANIMATED_LAYER_H_INCLUDED_*/
