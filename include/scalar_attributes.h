#pragma once
#include <vector>

class ScalarAttributes
{
public:
	ScalarAttributes(int size)
        : m_values(size),
		  m_minValue(INFINITY),
		  m_maxValue(-INFINITY) { }

	void setC0Scalar(int i, float v);
    float getC0Scalar(int i) { return m_values[i]; }
	float getMin() { return m_minValue; }
	float getMax() { return m_maxValue; }

protected:
    std::vector<float> m_values;
	float m_minValue;
	float m_maxValue;
};