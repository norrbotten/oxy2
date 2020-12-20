#include "renderer/netrender/httplib/http_request.hpp"

namespace Oxy::NetRender::HTTP {

  std::vector<std::string> split(const std::string& str, const std::string& delim,
                                 bool keep_empty = false) {
    std::vector<std::string> tokens;
    size_t                   prev = 0, pos = 0;

    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos)
        pos = str.length();

      std::string token = str.substr(prev, pos - prev);

      if (token.empty() && keep_empty)
        tokens.push_back("");
      else
        tokens.push_back(token);

      prev = pos + delim.length();

    } while (pos < str.length() && prev < str.length());

    return tokens;
  }

  HTTPRequest::HTTPRequest()
      : m_parse_failed(false) {}

  HTTPRequest::HTTPRequest(const void* request, std::size_t len)
      : m_parse_failed(false) {

    if (!parse(request, len)) {
      m_parse_failed = true;
    }
  }

  HTTPHeader HTTPRequest::header(const std::string& name) const {
    auto canon = HTTP::canonicalize(name);
    if (m_request_headers.contains(canon))
      return m_request_headers.at(canon);

    return HTTPHeader();
  }

  bool HTTPRequest::parse(const void* request, std::size_t len) {
    std::string s((char*)request, len);
    auto        split_by_crlf = split(s, "\r\n");

    if (split_by_crlf.size() == 0)
      return false;

    auto content_start_byte = split_by_crlf[0].length() + 2;

    auto request_line = split(split_by_crlf[0], " ");

    if (request_line.size() != 3)
      return false;

    auto method  = request_line[0];
    auto uri     = request_line[1];
    auto version = request_line[2];

    for (std::size_t i = 1; i < split_by_crlf.size(); i++) {
      if (split_by_crlf[i].length() == 0) {
        content_start_byte += 2;
        break;
      }
      else {
        content_start_byte += split_by_crlf[i].size() + 2;

        auto header     = split_by_crlf[i];
        auto name_value = split(header, ":");

        while (name_value.size() < 2)
          name_value.push_back("");

        if (name_value[1].length() > 1 && name_value[1].at(0) == ' ') {
          name_value[1] = name_value[1].substr(1);
        }

        m_request_headers[HTTP::canonicalize(name_value[0])] =
            HTTPHeader(name_value[0], name_value[1]);
      }
    }

    m_request_body = (content_start_byte > len) ? ""
                                                : std::string((char*)request + content_start_byte,
                                                              len - content_start_byte);

    if (auto pos = uri.find("?"); pos != std::string::npos) {
      m_uri = uri.substr(0, pos);

      if (uri.size() >= pos + 1) {
        m_query_string = uri.substr(pos + 1);
      }
      else {
        m_query_string = "";
      }
    }
    else {
      m_uri          = uri;
      m_query_string = "";
    }

    m_method       = method;
    m_http_version = version;

    bool valid_method = false;

    for (std::size_t i = 0; i < Method::__COUNT; i++) {
      if (STRINGIFY_METHOD[i] == m_method) {
        m_method_enum = (Method)i;
        valid_method  = true;
        break;
      }
    }

    if (!valid_method) {
      return false;
    }

    return true;
  }

  std::string HTTPRequest::format_into_request_str() const {
    std::stringstream ss;

    ss << m_method << " ";
    ss << m_path << " ";
    ss << HTTP_VERSION_STRING << "\r\n";

    for (auto& [name, header] : m_request_headers)
      ss << name << ":" << header.content() << "\r\n";

    ss << "\r\n";

    ss << m_request_body;

    return ss.str();
  }

} // namespace Oxy::NetRender::HTTP
