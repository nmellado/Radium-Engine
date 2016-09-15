#ifndef RADIUMENGINE_CHRONOMETER_DEFINITION
#define RADIUMENGINE_CHRONOMETER_DEFINITION

#include <Core/Time/Timer.hpp>

namespace Ra {
namespace Core {
namespace Timer {


/**
 * @brief The Chrono class represents a chronometer for timing generic functions
 * in an easy way.
 *
 * Example:
 *
 *      void foo( foo_args ... ) { ... }
 *      some_type bar( bar_args ... ) { ... }
 *
 *      Chrono foo_time;
 *      foo_time.run( foo, foo_args... );
 *
 *      Chrono bar_time;
 *      some_type bar_result = bar_time.run( bar, foo_args... );
 *
 *      if( foo_time == bar_time ) {
 *          std::cout << "time is equal";
 *      } else {
 *          if( foo_time < bar_time ) {
 *              std::cout << "foo is faster";
 *          } else {
 *              std::cout << "bar is faster";
 *          }
 *      }
 *
 * \note Note that bar( bar_args ...) == bar_time.run( bar, bar_args... )
 *
 */
class Chrono {
public:
    /**
     *    @brief Default constructor.
     */
    Chrono() {}

    /**
     *    @brief Copy constructor.
     */
    Chrono( const Chrono& other ) = default;

    /**
     *    @brief Move constructor.
     */
    Chrono( Chrono&& other ) = default;



    /**
     *    @brief Destructor.
     */
    ~Chrono() {}



    /**
     *    @brief Run the given void function f( args ... ) and times it.
     *    @param f                  The function to be timed.
     *    @param args               The parameters of f.
     */
    template< class Function, class... Args >
    inline void run( Function&& f, Args&&... args ) {
        m_start = Timer::Clock::now();
        f( args ... );
        m_end = Timer::Clock::now();
    }

    /**
     *    @brief Run the given ReturnType function f( args ... ) and times it.
     *    @param f                  The function to be timed.
     *    @param args               The parameters of f.
     *    @return The output of f( args ... ).
     */
    template < typename ReturnType, class Function,  class ... Args >
    inline ReturnType run( Function&& f, Args ... args ) {
        // TODO //static_assert( /*check if ReturnType is equal to Function return type*/, "RETURN_TYPE_DO_NOT_MATCH_FUNCTION_RETURN_TYPE" );
        m_start        = Timer::Clock::now();
        ReturnType res = f( args ... );
        m_end          = Timer::Clock::now();
        return res;
    }



    /**
     *    @brief Return the elapsed time for last call of run in microseconds.
     *    @return The elapsed time in microseconds.
     */
    inline long elapsedMicroSeconds() const {
        return Timer::getIntervalMicro( m_start, m_end );
    }

    /**
     *    @brief Return the elapsed time for last call of run in seconds.
     *    @return The elapsed time in seconds.
     */
    inline Scalar elapsedSeconds() const {
        return Timer::getIntervalSeconds( m_start, m_end );
    }



    /**
    *    @brief Copy assignment operator.
    */
    inline Chrono& operator=( const Chrono& other ) = default;

    /**
    *    @brief Move assignment operator.
    */
    inline Chrono& operator=( Chrono&& other ) = default;



    /**
    *    @brief Equal operator.
    */
    inline bool operator==( const Chrono& other ) const {
        return ( elapsedMicroSeconds() == other.elapsedMicroSeconds() );
    }

    /**
    *    @brief Less operator.
    */
    inline bool operator<( const Chrono& other ) const {
        return ( elapsedMicroSeconds() < other.elapsedMicroSeconds() );
    }



protected:
    /// VARIABLE
    Timer::TimePoint m_start; ///< Time at the beginning of the function.
    Timer::TimePoint m_end;   ///< Time after running the function.
};



} // namespace Timer
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_CHRONOMETER_DEFINITION