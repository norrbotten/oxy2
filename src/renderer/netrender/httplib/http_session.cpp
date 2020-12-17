#include "renderer/netrender/httplib/http_session.hpp"

namespace Oxy::NetRender::HTTP {

  HTTPSession::HTTPSession(TCP::Connection* connection)
      : m_done(false)
      , m_connection(connection) {}

  void HTTPSession::handle_request(const Endpoint& endpoint, const HTTPRequest& req,
                                   HTTPResponse& res) {

    // close the connection on error or if keep-alive wasnt passed

    // default
    // havent read any spec, but lets close the connection if there was a failure
    m_done = true;

    if (endpoint.is_valid()) {
      endpoint.call(req, res);

      if (res.header("Connection") == "Keep-Alive") {
        m_done = false;
      }

      if (m_done) {
        if (req.header("Connection") == "Keep-Alive")
          m_done = false;
      }
    }
  }

} // namespace Oxy::NetRender::HTTP
