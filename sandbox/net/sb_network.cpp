//
//  sb_network.cpp
//  Battleships
//
//  Created by Andrey Kunitsyn on 01/09/14.
//
//

#include "sb_network.h"
#include "sb_data.h"
#include <ghl_data.h>
#include "sb_log.h"
#include "sb_resources.h"

namespace Sandbox {
    
    NetworkRequestBase::NetworkRequestBase(const sb::string& url) : m_url(url),m_completed(false),m_error(false),m_received_size(0) {}
    NetworkRequestBase::~NetworkRequestBase() {
        
    }
    void GHL_CALL NetworkRequestBase::AddRef() const {
        add_ref();
    }
    /// release reference
    void GHL_CALL NetworkRequestBase::Release() const {
        remove_ref();
    }
    /// ghl interface
    /// url
    const char* GHL_CALL NetworkRequestBase::GetURL() const {
        return m_url.c_str();
    }
    /// headers
    GHL::UInt32 GHL_CALL NetworkRequestBase::GetHeadersCount() const {
        return GHL::UInt32(m_headers.size());
    }
    const char* GHL_CALL NetworkRequestBase::GetHeaderName(GHL::UInt32 idx) const {
        headers_map_t::const_iterator it = m_headers.begin();
        std::advance(it, idx);
        return it->first.c_str();
    }
    const char* GHL_CALL NetworkRequestBase::GetHeaderValue(GHL::UInt32 idx) const {
        headers_map_t::const_iterator it = m_headers.begin();
        std::advance(it, idx);
        return it->second.c_str();
    }
    
    /// received response
    void GHL_CALL NetworkRequestBase::OnResponse(GHL::UInt32 status) {
        
    }
    /// received header
    void GHL_CALL NetworkRequestBase::OnHeader(const char* name,const char* value) {
        m_received_headers[name]=value;
    }
    /// received data
    void GHL_CALL NetworkRequestBase::OnData(const GHL::Byte* data,GHL::UInt32 size) {
        m_received_size += size;
    }
    /// received complete
    void GHL_CALL NetworkRequestBase::OnComplete() {
        m_completed = true;
    }
    /// received error
    void GHL_CALL NetworkRequestBase::OnError(const char* error) {
        m_completed = true;
        m_error = true;
        if (error) {
            m_error_text = error;
        }
    }
    
    
    /// received data
    void GHL_CALL NetworkRequest::OnData(const GHL::Byte* data,GHL::UInt32 size) {
        NetworkRequestBase::OnData(data, size);
        m_data.append(reinterpret_cast<const char*>(data),size);
    }
    
    NetworkDataRequest::~NetworkDataRequest() {
        ReleaseData();
    }
    
    void NetworkDataRequest::OnData(const GHL::Byte *data, GHL::UInt32 size) {
        NetworkRequestBase::OnData(data, size);
        if (!m_data) {
            m_data = new VectorData<GHL::Byte>();
        }
        size_t pos = m_data->vector().size();
        m_data->vector().resize(pos+size);
        ::memcpy(&m_data->vector()[pos], data, size);
    }
    
    void NetworkDataRequest::ReleaseData() {
        if (m_data) {
            m_data->Release();
            m_data = 0;
        }
    }
#ifndef SB_NO_RESOURCES
    void GHL_CALL ImageRequest::OnComplete() {
        NetworkDataRequest::OnComplete();
        const GHL::Data* data = GetData();
        if (data) {
            m_img = m_resources->CreateImageFromData(data);
        }
        ReleaseData();
    }
#endif
    Network::Network(Resources* res) : m_resources(res) {
        m_net = GHL_CreateNetwork();
    }
    
    Network::~Network() {
        GHL_DestroyNetwork(m_net);
    }
    
    NetworkRequestPtr Network::SimpleGET(const sb::string& url) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        if (!m_net->Get(request.get()))
            return NetworkRequestPtr();
        return request;
    }
#ifndef SB_NO_RESOURCES
    ImageRequestPtr Network::GETImage(const sb::string& url) {
        if (!m_net || !m_resources) return ImageRequestPtr();
        ImageRequestPtr request(new ImageRequest(url,m_resources));
        if (!m_net->Get(request.get()))
            return ImageRequestPtr();
        return request;
    }
#endif
    NetworkRequestPtr Network::SimplePOST(const sb::string& url, const sb::string& data) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        GHL::Data* data_p = GHL_HoldData(reinterpret_cast<const GHL::Byte*>(data.c_str()), GHL::UInt32(data.length()));
        if (!m_net->Post(request.get(),data_p)) {
            if (!request->GetErrorText().empty()) {
                LogError() << "network POST error: " << request->GetErrorText();
            }
            data_p->Release();
            return request->GetCompleted() ? request : NetworkRequestPtr();
        }
        data_p->Release();
        return request;
    }
    
}
