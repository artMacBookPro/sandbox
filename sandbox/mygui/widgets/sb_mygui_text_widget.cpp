#include "sb_mygui_text_widget.h"
#include "../skins/sb_mygui_edit_text.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::TextWidget, MyGUI::TextBox)


namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(TextWidget)

        TextWidget::TextWidget() {
            
        }
        
        TextWidget::~TextWidget() {
            
        }
        
        bool TextWidget::getWordWrap() const {
            if (nullptr != getSubWidgetText())
                return getSubWidgetText()->getWordWrap();
            return false;
        }
        
        void TextWidget::setWordWrap(bool wrap) {
            if (nullptr != getSubWidgetText())
                getSubWidgetText()->setWordWrap(wrap);
        }
        
        void TextWidget::setLineSpacing(float _value)
        {
            if (nullptr != getSubWidgetText())
            {
                Sandbox::mygui::EditText* p_edit_text = dynamic_cast<Sandbox::mygui::EditText*>(getSubWidgetText());
                if (p_edit_text)
                    p_edit_text->setLineSpacing(_value);
            }
        }
        
        float TextWidget::getLineSpacing() const{
            if (nullptr != getSubWidgetText())
            {
                const Sandbox::mygui::EditText* p_edit_text = dynamic_cast<const Sandbox::mygui::EditText*>(getSubWidgetText());
                if (p_edit_text)
                    return p_edit_text->getLineSpacing();
            }
            return 1.0;
        }

        
        void TextWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "WordWrap") {
                setWordWrap(MyGUI::utility::parseValue<bool>(_value));
            }
            else
            if (_key == "LineSpacing") {
                setLineSpacing(MyGUI::utility::parseValue<float>(_value));
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
    }

}
