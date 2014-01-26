//
//  sb_mygui_render.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 25/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_render__
#define __sr_osx__sb_mygui_render__

#include "MyGUI_RenderManager.h"
#include "MyGUI_IRenderTarget.h"
#include <sbstd/sb_list.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

namespace GHL {
    struct Render;
}

namespace Sandbox {
    
    class Resources;
    
    namespace mygui {
        
        class RenderManager : public MyGUI::RenderManager, public MyGUI::IRenderTarget {
        public:
            explicit RenderManager( Resources* resources );
            /** Create vertex buffer.
             This method should create vertex buffer with triangles list type,
             each vertex have position, colour, texture coordinates.
             */
            virtual MyGUI::IVertexBuffer* createVertexBuffer();
            /** Destroy vertex buffer */
            virtual void destroyVertexBuffer(MyGUI::IVertexBuffer* _buffer);
            
            /** Create empty texture instance */
            virtual MyGUI::ITexture* createTexture(const std::string& _name);
            /** Destroy texture */
            virtual void destroyTexture(MyGUI::ITexture* _texture);
            /** Get texture by name */
            virtual MyGUI::ITexture* getTexture(const std::string& _name);
            
            //FIXME возможно перенести в структуру о рендер таргете
            virtual const MyGUI::IntSize& getViewSize() const;
            
            /** Get current vertex colour type */
            virtual MyGUI::VertexColourType getVertexFormat();
            
            /** Check if texture format supported by hardware */
            virtual bool isFormatSupported(MyGUI::PixelFormat _format, MyGUI::TextureUsage _usage);
            
#if MYGUI_DEBUG_MODE == 1
            /** Check if texture is valid */
            virtual bool checkTexture(MyGUI::ITexture* _texture);
#endif
            
            virtual void begin();
            virtual void end();
            
            virtual void doRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count);
            
            virtual const MyGUI::RenderTargetInfo& getInfo();
            
            void    drawFrame(float dt);
        private:
            Resources*      m_resources;
            MyGUI::IntSize  m_view_size;
            class Texture;
            sb::map<sb::string,Texture*>  m_textures;
            MyGUI::RenderTargetInfo m_rt_info;
        };
        
    }
    
}

#endif /* defined(__sr_osx__sb_mygui_render__) */