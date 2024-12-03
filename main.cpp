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
#include <iostream>

int main(int argc, char** argv)
{
    // The test player. Example taken from
    // http://www.glicko.net/glicko/glicko2.pdf
    Glicko::Rating player(1500, 200);
    // The three opponents
    const Glicko::Rating players[3] = 
    {
        Glicko::Rating(1400, 30),
        Glicko::Rating(1550, 100),
        Glicko::Rating(1700, 300)
    };

    // Simulate 3 games and update the ratings
    const Glicko::real_type scores[3] = {1.0, 0.0, 0.0};
    player.Update(3, players, scores);
    player.Apply();
    
    std::cout << player << std::endl;

    return 0;
}
