#version 430 core

uniform uvec2 windowSize;
uniform double zoomScale;
uniform dvec2 numberStart;
uniform uint maxIterations = 400;

out vec4 fragColor;

uint calcMandel(double startReal, double startImag) {

    double real = startReal;
    double imag = startImag;

    for (int n = 1; n < maxIterations; n++) {
        if ((real * real) + (imag * imag) > 4) {
            return n;
        }
        double realTemp = real;

        real = (real * real) - (imag * imag) + startReal;
        imag = 2 * realTemp * imag + startImag;
    }
    return 0;
}

vec4 flowColor(uint index) {
    float r = 0.0;
    float g = 1.0;
    float b = 0.5333;

    const uint colorAccuracy = 10; // 1 means every index results in a colorStep of (1.0 / 1) but there are only (6 * 1) colors. 255 means, that every index results in a much smaller colorStep of (1.0 / 255) but there are (255 * 6) colors. 
    float colorStep = float(index % (colorAccuracy * 6)) / (colorAccuracy);

    while (true) {
        if (r == 1.0 && g < 1.0 && b == 0.0) {
            if (g + colorStep > 1.0) {
                colorStep -= (1.0 - g);
                g = 1.0;
            }
            else {
                g += colorStep;
                break;
            }
        }
        else if (r > 0.0 && g == 1.0) {
            if (r - colorStep < 0.0) {
                colorStep -= r;
                r = 0.0;
            }
            else {
                r -= colorStep;
                break;
            }
        }
        else if (g == 1.0 && b < 1.0) {
            if (b + colorStep > 1.0) {
                colorStep -= (1.0 - b);
                b = 1.0;
            }
            else {
                b += colorStep;
                break;
            }
        }
        else if (g > 0.0 && b == 1.0) {
            if (g - colorStep < 0.0) {
                colorStep -= g;
                g = 0.0;
            }
            else {
                g -= colorStep;
                break;
            }
        }
        else if (b == 1.0 && r < 1.0) {
            if (r + colorStep > 1.0) {
                colorStep -= (1.0 - r);
                r = 1.0;
            }
            else {
                r += colorStep;
                break;
            }
        }
        else if (b > 0.0 && r == 1.0) {
            if (b - colorStep < 0.0) {
                colorStep -= b;
                b = 0.0;
            }
            else {
                b -= colorStep;
                break;
            }
        }
    }
    return vec4(r, g, b, 1.0);
}

void main() {
    double real = zoomScale * (double(gl_FragCoord.x) + 0.5) / windowSize.x + numberStart.x;
    double imag = (zoomScale * (double(gl_FragCoord.y) + 0.5) + numberStart.y * windowSize.y) / windowSize.x;
    
    uint calc = calcMandel(real, imag);
    if (calc == 0)
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
        fragColor = flowColor(calc);
}
