#pragma once

#include <deque>
#include <vector>

#include "renderer/integrators/integrator.hpp"

namespace Oxy::Integrators {

  /*
    PSSMLT implementation from smallpaint, just refactored into reasonable c++
    I am also using a deque for the state stack since it has a clear method
    and PSSMLTPath::restore_state isnt as dumb
  */

  struct PSSMLTPathPoint {
    FloatType             value;
    int                   modify;
    std::deque<FloatType> s;
  };

  class PSSMLTPath {
  public:
    PSSMLTPath() {
      add(random<FloatType>(0, 1), 0);
      add(random<FloatType>(0, 1), 0);
    }

    auto get_value(std::size_t i) const { return m_points[i].value; }
    auto get_modify(std::size_t i) const { return m_points[i].modify; }

    void set_value(std::size_t i, FloatType value) { m_points[i].value = value; }
    void set_modify(std::size_t i, int modify) { m_points[i].modify = modify; }

    auto size() const { return m_points.size(); }

    void add(FloatType value, int modify) {
      m_points.push_back(PSSMLTPathPoint{value, modify});
      m_points.back().s.push_back(value);
    }

    void save_state(std::size_t i, FloatType value) { m_points[i].s.push_back(value); }

    void clear_stack() {
      for (auto& point : m_points)
        point.s.clear();
    }

    void restore_state() {
      for (auto& point : m_points) {
        point.value = point.s.front();
        point.s.clear();
      }
    }

    void reset_modify() {
      for (auto& point : m_points)
        point.modify = 0;
    }

  private:
    std::vector<PSSMLTPathPoint> m_points;
  };

  struct PSSMLTContribution {
    int   x, y;
    Color color;
  };

  struct PSSMLTSample {
    FloatType          weight;
    PSSMLTContribution contrib;
  };

  class PSSMLTSampler {
  public:
    PSSMLTSampler(const PSSMLTPath& path, FloatType b, int m, int large_step)
        : m_path(path)
        , m_old_sample{0, {0, 0, Color(0.0)}}
        , m_old_i(0)
        , m_b(b)
        , m_m(m)
        , m_large_step(large_step)
        , m_time(large_step)
        , m_large_step_time(large_step) {}

    FloatType mutate(FloatType value) {
      constexpr FloatType s1 = 1.0 / 1024.0;
      constexpr FloatType s2 = 1.0 / 64.0;
      const FloatType     dv = s2 * glm::exp(-glm::log(s2 / s1) * random<FloatType>(0, 1));

      if (random<FloatType>(0, 1) < 0.5) {
        value += dv;
        if (value > 1)
          value -= 1;
      }
      else {
        value -= dv;
        if (value < 0)
          value += 1;
      }

      return value;
    }

    FloatType primary_sample(std::size_t i) {
      if (i >= m_path.size()) {
        while (i >= m_path.size())
          m_path.add(random<FloatType>(0, 1), m_time);
      }
      else {
        if (m_path.get_modify(i) < m_time) {
          if (m_large_step) {
            m_path.save_state(i, m_path.get_value(i));
            m_path.set_modify(i, m_time);
            m_path.set_value(i, random<FloatType>(0, 1));
          }
          else {
            if (m_path.get_modify(i) < m_large_step_time) {
              m_path.set_modify(i, m_large_step_time);
              m_path.set_value(i, random<FloatType>(0, 1));
            }

            while (m_path.get_modify(i) < m_time - 1) {
              m_path.set_value(i, mutate(m_path.get_value(i)));
              m_path.set_modify(i, m_path.get_modify(i) + 1);
            }

            m_path.save_state(i, m_path.get_value(i));
            m_path.set_value(i, mutate(m_path.get_value(i)));
            m_path.set_modify(i, m_path.get_modify(i) + 1);
          }
        }
      }

      return m_path.get_value(i);
    }

    PSSMLTSample next(FloatType i, const PSSMLTContribution& contrib) {
      auto a = std::min(1.0, i / m_old_i);

      PSSMLTSample new_sample;
      PSSMLTSample contrib_sample;

      new_sample.contrib = contrib;
      new_sample.weight  = (a + m_large_step) / (i / m_b + 0.5) / m_m;

      m_old_sample.weight += (1.0 - a) / (m_old_i / m_b + 0.5) / m_m;

      if (random<FloatType>(0, 1) < a) {
        m_old_i        = i;
        contrib_sample = m_old_sample;
        m_old_sample   = new_sample;

        if (m_large_step)
          m_large_step_time = m_time;

        m_time++;

        m_path.clear_stack();
      }
      else {
        contrib_sample = new_sample;
        m_path.restore_state();
      }

      m_large_step = random<FloatType>(0, 1) < 0.5;

      return contrib_sample;
    }

  private:
    PSSMLTPath   m_path;
    PSSMLTSample m_old_sample;

    FloatType m_old_i;
    FloatType m_b;

    int m_m; // mmmhmmm
    int m_large_step;
    int m_time;
    int m_large_step_time;
  };

  class PSSMLTIntegrator : public Integrator {
  public:
  private:
    PSSMLTSampler m_sampler;
  };

} // namespace Oxy::Integrators
