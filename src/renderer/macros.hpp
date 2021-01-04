#pragma once

#define REF(field)                                                                                 \
  auto& field() { return m_##field; }

#define CREF(field)                                                                                \
  const auto& field() const { return m_##field; }
