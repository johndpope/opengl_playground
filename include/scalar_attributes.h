#pragma once
#include <vector>

class ScalarAttributes
{
public:
	ScalarAttributes(int size)
        : m_values(size) { }

    void setC0Scalar(int i, float v) { m_values[i] = v; }
    float getC0Scalar(int i) { return m_values[i]; }
    float getMin();
    float getMax();

protected:
    std::vector<float> m_values;
};