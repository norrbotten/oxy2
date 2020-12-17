#include "renderer/netrender/httplib/http_response.hpp"

#include <sstream>

namespace Oxy::NetRender::HTTP {

  HTTPResponse::HTTPResponse(TCP::Connection* connection)
      : m_connection(connection)
      , m_finalized(false)
      , m_sent(false)
      , m_status(STATUS_CODES.at(200)) {}

  void HTTPResponse::strip_body() { m_response_body.clear(); }

  HTTPResponse& HTTPResponse::write_header(const std::string& name, const std::string& payload) {
    if (m_finalized)
      throw ResponseAlreadyFinalized();

    m_response_headers[HTTP::canonicalize(name)] = HTTPHeader(name, payload);

    return *this;
  }

  HTTPResponse& HTTPResponse::write_body(const std::string& payload) {
    if (m_finalized)
      throw ResponseAlreadyFinalized();

    m_response_body += payload;

    return *this;
  }

  HTTPResponse& HTTPResponse::status(ssize_t status) {
    if (m_finalized)
      throw ResponseAlreadyFinalized();

    if (STATUS_CODES.contains(status)) {
      m_status = STATUS_CODES.at(status);
    }
    else {
      m_status.code     = status;
      m_status.code_str = "Unknown status";
    }

    return *this;
  }

  HTTPResponse& HTTPResponse::finalize() {
    m_finalized = true;
    return *this;
  }

  void HTTPResponse::send() {
    if (!m_finalized)
      finalize();

    std::stringstream response;

    response << HTTP_VERSION_STRING << " ";
    response << m_status.code << " " << m_status.code_str << "\r\n";

    for (auto& [name, header] : m_response_headers)
      response << header.name() << ": " << header.content() << "\r\n";

    response << "\r\n";

    if (!m_response_body.empty())
      response << m_response_body;

    m_connection->write((char*)response.str().c_str(), response.str().size());
  }

  HTTPHeader HTTPResponse::header(const std::string& name) const {
    if (m_response_headers.contains(HTTP::canonicalize(name))) {
      return m_response_headers.at(HTTP::canonicalize(name));
    }

    return HTTPHeader();
  }

} // namespace Oxy::NetRender::HTTP
