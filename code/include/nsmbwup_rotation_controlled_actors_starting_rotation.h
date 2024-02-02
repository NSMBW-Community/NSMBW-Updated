// MIT License

// Copyright (c) 2021 RoadrunnerWMC

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


// Most rotation-controlled things inherit from daRotObjsBase_c and use
// its correctly-written functions for reading the rotation controller's
// info. But some handle it all manually themselves instead, and several
// don't read the starting-rotation value properly.

// All these actors have an "initial angle" field, representing their
// placement angle from the controller, calculated with atan2 upon load.
// The mistake they make is trying to also incorporate the controller's
// starting-rotation value into that, which is wrong because that's
// already included in the controller's rotation values themselves.

// daEnLiftRotHalf_c subtracts the value away, which results in that
// actor effectively ignoring starting-rotation. All the others add it
// instead, causing the starting-rotation to double.

// -0x4000 is used in these patches as a replacement for instructions
// that read the rot. controller's starting-rotation value, because it's
// the difference between "up" (the default angle) and "right" (0
// radians). This constant shows up frequently in Nintendo's own
// rotation-control code, too.
