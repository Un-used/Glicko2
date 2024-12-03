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

#ifndef GLICKO_RATING_HPP
#define GLICKO_RATING_HPP

#include "glicko/config.hpp"
#include <cstddef>
#include <ostream>

// Forward declaration
namespace Glicko { class Rating; }

/// Outputs the rating in Glicko-1 fromat
std::ostream& operator<<(
    std::ostream& os,
    const Glicko::Rating& rating);

namespace Glicko
{

    /// Defines a struct that contains Glicko rating parameters Parameters are
    /// stored in the Glicko-2 rating scale, but can also be queried in in
    /// original Glicko scale.
    class Rating
    {
    public:
        /// Constructs a rating with from a desired rating and deviation.
        /// Defaults to the Glicko-1 standard of R = 1500, RD = 350.
        Rating(
            real_type rating = Glicko::kDefaultR,
            real_type deviation = Glicko::kDefaultRD,
            real_type volatility = Glicko::kDefaultS) noexcept;

        /// Constructs a rating from another rating instance
        Rating(
            const Rating& rating) noexcept;
        
        /// Updates the rating based on a set of games
        void Update(
            std::size_t m, 
            const Rating* opponents, 
            const real_type* score);

        // Updates the rating based on a single game
        void Update(
            const Rating& opponent, 
            real_type score) noexcept;

        // Decays the rating deviation
        void Decay() noexcept;
        
        /// Applies the updated ratings
        void Apply() noexcept;

        /// Returns the Glicko-1 rating
        real_type Rating1() const noexcept;

        /// Returns the Glicko-1 deviation
        real_type Deviation1() const noexcept;

        /// Returns the Glicko-2 rating
        real_type Rating2() const noexcept;

        /// Returns the Glicko-2 deviation
        real_type Deviation2() const noexcept;

        /// Outputs the rating in Glicko-1 fromat
        friend std::ostream& (::operator<<)(
            std::ostream& os,
            const Rating& rating);

    private:
        void Update(
            real_type invV,
            real_type v,
            real_type dInner,
            real_type d) noexcept;

        /// Computes the value of the g function for a rating
        real_type G() const noexcept;

        /// Computes the value of the e function in terms of a g function value
        /// and another rating
        real_type E(
            real_type g, 
            const Rating& rating) const noexcept;

        /// Computes the value of the f function in terms of x, delta^2, phi^2,
        /// v, a and tau^2.
        static real_type F(
            real_type x,
            real_type dS,
            real_type pS,
            real_type v,
            real_type a,
            real_type tS) noexcept;

        /// Performs convergence iteration on the function f
        static real_type Convergence(
            real_type d,
            real_type v,
            real_type p,
            real_type s) noexcept;

        /// The rating u (mu)
        real_type u;

        /// The rating deviation p (phi)
        real_type p;

        /// The rating volatility s (sigma)
        real_type s;

        /// The pending rating value, u'
        real_type uPrime;

        /// The pending deviation value, p'
        real_type pPrime;

        /// The pending volatility value, s'
        real_type sPrime;
    };

} // namespace Glicko

#endif // GLICKO_RATING_HPP
