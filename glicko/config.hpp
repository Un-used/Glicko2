/*
*   The MIT License (MIT)
*
*   Copyright (c) 2015 Taylor Petrick
*   Copyright (c) 2024 Kim Jae-hyeon
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*/

#ifndef GLICKO_CONFIG_HPP
#define GLICKO_CONFIG_HPP

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#include <version>
#endif // __cplusplus

#if __cpp_lib_math_constants >= 201907L
#include <numbers>
#else // __cpp_lib_math_constants
#define _USE_MATH_DEFINES
#include <cmath>
#endif // __cpp_lib_math_constants

#define GLICKO_ENABLE_MEMORY_REUSE 1

namespace Glicko
{

    using real_type = double;

    inline constexpr real_type kPI = 
#if __cpp_lib_math_constants >= 201907L
        std::numbers::pi_v<real_type>;
#else // __cpp_lib_math_constants
        static_cast<real_type>(M_PI);
#endif // __cpp_lib_math_constants

    /// The default/initial rating value
    inline constexpr real_type kDefaultR = static_cast<real_type>(1500.0L);
    
    /// The default/initial deviation value1
    inline constexpr real_type kDefaultRD = static_cast<real_type>(350.0L);

    /// The default/initial volatility value
    inline constexpr real_type kDefaultS = static_cast<real_type>(0.06L);


    /// The Glicko-1 to Glicko-2 scale factor
    inline constexpr real_type kScale = static_cast<real_type>(173.7178L);

    /// The system constant (tau)
    inline constexpr real_type kSystemConst = static_cast<real_type>(0.5L);

    /// The convergence constant (epsilon)
    inline constexpr real_type kConvergence = static_cast<real_type>(0.000001L);

} // namespace Glicko

#endif // GLICKO_CONFIG_HPP
