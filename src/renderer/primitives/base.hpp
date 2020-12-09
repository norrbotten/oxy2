#pragma once

#include "renderer/types.hpp"

namespace Oxy::Primitive {

  /*
    Base primitive class
    Polymorphism is achieved with template specialization
  */
  enum class PrimitiveType {
    Sphere,
  };

  template <PrimitiveType Type>
  class PrimitiveImpl final {
  public:
    PrimitiveImpl()  = delete;
    ~PrimitiveImpl() = delete;
  };

} // namespace Oxy::Primitive
