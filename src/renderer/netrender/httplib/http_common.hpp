#pragma once

#include <array>
#include <unordered_map>

namespace Oxy::NetRender::HTTP {

  constexpr auto HTTP_VERSION_STRING = "HTTP/1.0";

  struct StatusCode {
    ssize_t     code     = 200;
    const char* code_str = "OK";
  };

  /* clang - format off */
#define HTTP_STATUS(code, str)                                                                     \
  {                                                                                                \
    code, StatusCode { code, str }                                                                 \
  }

  const std::unordered_map<ssize_t, StatusCode> STATUS_CODES = {
      HTTP_STATUS(100, "Continue"),
      HTTP_STATUS(101, "Switching Protocols"),
      HTTP_STATUS(102, "Processing"),
      HTTP_STATUS(103, "Early Hints"),

      HTTP_STATUS(200, "OK"),
      HTTP_STATUS(201, "Created"),
      HTTP_STATUS(202, "Accepted"),
      HTTP_STATUS(203, "Non-Authoritative Information"),
      HTTP_STATUS(204, "No Content"),
      HTTP_STATUS(205, "Reset Content"),
      HTTP_STATUS(206, "Partial Content"),
      HTTP_STATUS(207, "Multi-Status"),
      HTTP_STATUS(208, "Already Reported"),
      HTTP_STATUS(209, "IM Used"),

      HTTP_STATUS(300, "Multiple Choices"),
      HTTP_STATUS(301, "Moved Permanently"),
      HTTP_STATUS(302, "Found"),
      HTTP_STATUS(303, "See Other"),
      HTTP_STATUS(304, "Not Modified"),
      HTTP_STATUS(305, "Use Proxy"),
      HTTP_STATUS(306, "Switch Proxy"),
      HTTP_STATUS(307, "Temporary Redirect"),
      HTTP_STATUS(308, "Permanent Redirect"),

      HTTP_STATUS(400, "Bad Request"),
      HTTP_STATUS(401, "Unauthorized"),
      HTTP_STATUS(402, "Payment Required"), // stonks
      HTTP_STATUS(403, "Forbidden"),
      HTTP_STATUS(404, "Not Found"),
      HTTP_STATUS(405, "Method Not Allowed"),
      HTTP_STATUS(406, "Not Acceptable"),
      HTTP_STATUS(407, "Proxy Authentication Required"),
      HTTP_STATUS(408, "Request Timeout"),
      HTTP_STATUS(409, "Conflict"),
      HTTP_STATUS(410, "Gone"),
      HTTP_STATUS(411, "Length Required"),
      HTTP_STATUS(412, "Precondition Failed"),
      HTTP_STATUS(413, "Payload Too Large"),
      HTTP_STATUS(414, "URI Too Long"),
      HTTP_STATUS(415, "Unsupported Media Type"),
      HTTP_STATUS(416, "Range Not Satisfiable"),
      HTTP_STATUS(417, "Expectation Failed"),
      HTTP_STATUS(418, "I'm a teapot"),
      HTTP_STATUS(421, "Misdirected Request"),
      HTTP_STATUS(422, "Unprocessable Entity"),
      HTTP_STATUS(423, "Locked"),
      HTTP_STATUS(424, "Failed Dependency"),
      HTTP_STATUS(425, "Too Early"),
      HTTP_STATUS(426, "Upgrade Required"),
      HTTP_STATUS(428, "Precondition Failed"),
      HTTP_STATUS(429, "Too Many Requests"),
      HTTP_STATUS(431, "Request Header Fields Too Large"),
      HTTP_STATUS(451, "Unavailable For Legal Reasons"),

      HTTP_STATUS(500, "Internal Server Error"),
      HTTP_STATUS(501, "Not Implemented"),
      HTTP_STATUS(502, "Bad Gateway"),
      HTTP_STATUS(503, "Service Unavailable"),
      HTTP_STATUS(504, "Gateway Timeout"),
      HTTP_STATUS(505, "HTTP Version Not Supported"),
      HTTP_STATUS(506, "Variant Also Negotiates"),
      HTTP_STATUS(507, "Insufficient Storage"),
      HTTP_STATUS(508, "Loop Detected"),
      HTTP_STATUS(510, "Not Extended"),
      HTTP_STATUS(511, "Network Authentication Required"),
  };
  /* clang - format on */

  enum Method {
    GET     = 0,
    HEAD    = 1,
    POST    = 2,
    PUT     = 3,
    DELETE  = 4,
    CONNECT = 5,
    OPTIONS = 6,
    TRACE   = 7,
    PATCH   = 8,
    __COUNT,
  };

  /* clang - format off */

  constexpr std::array<const char*, Method::__COUNT> STRINGIFY_METHOD = {
      "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH",
  };

  /* clang - format on */

} // namespace Oxy::NetRender::HTTP
