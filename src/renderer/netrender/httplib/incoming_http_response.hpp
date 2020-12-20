#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "renderer/netrender/httplib/http_header.hpp"
#include "renderer/netrender/httplib/http_request.hpp"

namespace Oxy::NetRender::HTTP {

  class IncomingHTTPResponse {
  public:
    IncomingHTTPResponse()
        : m_done(false) {}

    IncomingHTTPResponse(const std::string& data) {
      std::size_t pos = 0;

      auto crap = split(data, "\r\n", true);

      if (crap.size() > 0) {
        auto status_line = split(crap.at(0), " ", true);
        pos += crap.at(0).size() + 2;

        std::size_t       status_num;
        std::stringstream ss;
        ss << status_line.at(1);
        ss >> status_num;

        if (STATUS_CODES.contains(status_num))
          m_status = STATUS_CODES.at(status_num);

        for (int i = 1; i < crap.size(); i++) {
          if (crap.at(i).size() == 0) {
            pos += 2;
            break;
          }

          auto header = split(crap.at(i), ": ", true);

          if (header.size() == 2)
            m_headers[header.at(0)] = HTTPHeader(header.at(0), header.at(1));
          else if (header.size() == 1)
            m_headers[header.at(0)] = HTTPHeader(header.at(0), "");

          pos += crap.at(i).size() + 2;
        }
      }

      if (pos < data.size()) {
        m_body = data.substr(pos);
      }

      if (auto head = header("Content-Length"); head.valid()) {
        int i = 0;
        sscanf(head.content().c_str(), "%d", &i);

        if (i <= m_body.size())
          m_done = true;
      }
    }

    HTTPHeader header(const std::string& name) const {
      auto canon = HTTP::canonicalize(name);
      if (m_headers.contains(canon))
        return m_headers.at(canon);

      return HTTPHeader();
    }

    auto done() const { return m_done; }

    const auto& status() const { return m_status; }
    const auto& body() const { return m_body; }

  private:
    bool m_done;

    StatusCode m_status;

    std::unordered_map<std::string, HTTPHeader> m_headers;
    std::string                                 m_body;
  };

} // namespace Oxy::NetRender::HTTP
