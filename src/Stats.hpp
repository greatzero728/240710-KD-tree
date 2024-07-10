
#ifndef STATS_HPP
#define STATS_HPP

#include <cstdio>
namespace CS350 {
    /**
     * Debug structure, keeps track of how many times a certain operation was executed
     */
    class Stats {
      private:
        Stats() { Reset(); }
        ~Stats()                       = default;
        Stats(Stats const&)            = delete;
        Stats& operator=(Stats const&) = delete;

      public:
        static Stats& Instance() {
            static Stats st;
            return st;
        }

        void Reset() {
            frustumVsAabb = 0;
            aabbVsAabb    = 0;
            rayVsAabb     = 0;
            rayVsTriangle = 0;
        }

        size_t frustumVsAabb{};
        size_t aabbVsAabb{};
        size_t rayVsAabb{};
        size_t rayVsTriangle{};
    };
}
#endif // STATS_HPP
