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

#include "glicko/rating.hpp"
#include <cassert>
#include <memory>

namespace Glicko
{

    Rating::Rating(
        const real_type rating,
        const real_type deviation,
        const real_type volatility) noexcept
        : u((rating - Glicko::kDefaultR) / Glicko::kScale)
        , p(deviation / Glicko::kScale)
        , s(volatility)
    {
    }

    Rating::Rating(
        const Rating& rating) noexcept
        : u(rating.u)
        , p(rating.p)
        , s(rating.s)
    {
    }

    void Rating::Update(
        const std::size_t m, 
        const Rating* const opponents, 
        const real_type* const score)
    {
        assert(m > 0u && opponents && score);

#if defined(GLICKO_ENABLE_MEMORY_REUSE) && GLICKO_ENABLE_MEMORY_REUSE != 0
        thread_local std::unique_ptr<real_type[]> gTable;
        thread_local std::unique_ptr<real_type[]> eTable;
        thread_local std::size_t tableSize = 0u;
        if (tableSize < m)
        {
            std::unique_ptr<real_type[]> gTableNew(new real_type[m]);
            std::unique_ptr<real_type[]> eTableNew(new real_type[m]);
            gTable = std::move(gTableNew);
            eTable = std::move(eTableNew);
            tableSize = m;
        }
#else // GLICKO_ENABLE_MEMORY_REUSE
        std::unique_ptr<real_type[]> gTable(new real_type[m]);
        std::unique_ptr<real_type[]> eTable(new real_type[m]);
#endif // GLICKO_ENABLE_MEMORY_REUSE

        real_type invV = static_cast<real_type>(0.0L);
    
        // Compute the g and e values for each opponent and 
        // accumulate the results into the v value
        for (std::size_t j = 0u; j < m; ++j)
        {
            const Rating& opponent = opponents[j];

            const real_type g = opponent.G();
            const real_type e = opponent.E(g, *this);

            gTable[j] = g;
            eTable[j] = e;
            invV += g * g * e * (static_cast<real_type>(1.0L) - e); 
        }

        // Invert the v value
        const real_type v = static_cast<real_type>(1.0L) / invV;

        // Compute the delta value from the g, e, and v
        // values
        real_type dInner = static_cast<real_type>(0.0L);
        for (std::size_t j = 0u; j < m; ++j)
        {
            dInner += gTable[j] * (score[j] - eTable[j]);
        }

        // Apply the v value to the delta
        const real_type d = v * dInner;

        Update(invV, v, dInner, d);
    }

    void Rating::Update(
        const Rating& opponent, 
        const real_type score) noexcept
    {
        // Compute the e and g function values
        const real_type g = opponent.G();
        const real_type e = opponent.E(g, *this);

        // Compute 1/v and v
        const real_type invV = g * g * e * (static_cast<real_type>(1.0L) - e);
        const real_type v = static_cast<real_type>(1.0L) / invV;

        // Compute the delta value from the g, e, and v
        // values
        const real_type dInner = g * (score - e);
        const real_type d = v * dInner;

        Update(invV, v, dInner, d);
    }

    void Rating::Update(
        const real_type invV,
        const real_type v,
        const real_type dInner,
        const real_type d) noexcept
    {
        // Compute new rating, deviation and volatility values
        sPrime = std::exp(Convergence(d, v, p, s)/static_cast<real_type>(2.0L));
        pPrime = static_cast<real_type>(1.0L) / sqrt((static_cast<real_type>(1.0L)/(p*p + sPrime*sPrime)) + invV);
        uPrime = u + pPrime * pPrime * dInner;
    }

    void Rating::Decay() noexcept
    {
        // Decay the deviation if no games were played
        pPrime = std::sqrt(p*p + s*s);
    }

    void Rating::Apply() noexcept
    {
        // Assign the new pending values to the actual rating values
        u = uPrime;
        p = pPrime;
        s = sPrime;
    }

    real_type Rating::Rating1() const noexcept
    {
        return (u * Glicko::kScale) + Glicko::kDefaultR;
    }

    real_type Rating::Deviation1() const noexcept
    {
        return p * Glicko::kScale;
    }

    real_type Rating::Rating2() const noexcept
    {
        return u;
    }

    real_type Rating::Deviation2() const noexcept
    {
        return p;
    }

    real_type Rating::G() const noexcept
    {
        const real_type scale = p / kPI;
        return static_cast<real_type>(1.0L) / std::sqrt(static_cast<real_type>(1.0L) + static_cast<real_type>(3.0L) * scale * scale);
    }

    real_type Rating::E(
        const real_type g, 
        const Rating& rating) const noexcept
    {
        const real_type exponent = static_cast<real_type>(-1.0L) * g * (rating.u - u);
        return static_cast<real_type>(1.0L) / (static_cast<real_type>(1.0L) + std::exp(exponent));
    }

    real_type Rating::F(
        const real_type x,
        const real_type dS,
        const real_type pS,
        const real_type v,
        const real_type a,
        const real_type tS) noexcept
    {
        const real_type eX = std::exp(x);
        const real_type num = eX * (dS - pS - v - eX);
        const real_type den = pS + v + eX;
        return (num / (static_cast<real_type>(2.0L) * den * den)) - ((x - a) / tS);
    }

    real_type Rating::Convergence(
        const real_type d,
        const real_type v,
        const real_type p,
        const real_type s) noexcept
    {
        // Initialize function values for iteration procedure
        const real_type dS = d*d;
        const real_type pS = p*p;
        const real_type tS = Glicko::kSystemConst * Glicko::kSystemConst;
        const real_type a = std::log(s*s);

        // Select the upper and lower iteration ranges
        real_type A = a;
        real_type B;
        const real_type bTest = dS - pS - v;

        if (bTest > 0.0) // ∆2 > φ2 + v
        {
            B = std::log(bTest); // B = ln(∆2 − φ2 − v)
        }
        else  // ∆2 ≤ φ2 + v
        {
            B = a - Glicko::kSystemConst;
            while (F(B, dS, pS, v, a, tS) < 0.0)
            {
                B -= Glicko::kSystemConst;
            }
        }

        // Perform the iteration
        real_type fA = F(A, dS, pS, v, a, tS);
        real_type fB = F(B, dS, pS, v, a, tS);
        while (std::fabs(B - A) > Glicko::kConvergence)
        {
            real_type C = A + (A - B) * fA / (fB - fA);
            real_type fC = F(C, dS, pS, v, a, tS);

            if (fC * fB < 0.0)
            {
                A = B;
                fA = fB;
            }
            else
            {
                fA /= 2.0;
            }

            B = C;
            fB = fC;
        }

        return A;
    }

} // namespace Glicko

std::ostream& operator<<(
    std::ostream& os,
    const Glicko::Rating& rating)
{
    os << '[' << rating.Rating1() << ':' << rating.Deviation1() << ']';
    return os;
}
